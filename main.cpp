#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include <vector>

const int CANVAS_WIDTH = 600;
const int CANVAS_HEIGHT = 600;

struct Vec3 {
    float x, y, z;
};

struct Vec2 {
    float x, y;
};

const float lado = 0.5f;
const std::vector<Vec3> vertices = {
    {lado, lado, lado},
    {-lado, lado, lado},
    {-lado, -lado, lado},
    {lado, -lado, lado},

    {lado, lado, -lado},
    {-lado, lado, -lado},
    {-lado, -lado, -lado},
    {lado, -lado, -lado},
};

// temp
const std::vector<std::vector<int>> faces = {
    {0, 1, 2, 3}, // front
    {4, 5, 6, 7}, // back
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7}
};

Vec2 cartesian(const Vec2& p) {
    const float nx  = (p.x + 1) / 2;
    const float ny = (p.y + 1) / 2;

    return {nx * CANVAS_WIDTH, ny * CANVAS_HEIGHT};
}

Vec2 project(const Vec3& p) {
    if(p.z == 0) {
        return {0, 0};
    }

    return {p.x / p.z, p.y / p.z};
}

Vec3 rotateY(const Vec3& p, float angle) {
    return {
        p.x * std::cos(angle) - p.z * std::sin(angle),
        p.y,
        p.x * std::sin(angle) + p.z * std::cos(angle)
    };
}

void setPixel(Uint32* pixels, int x, int y, Uint32 color) {
    if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) { // kinda slow?
        pixels[y * CANVAS_WIDTH + x] = color;
    }
}

void drawPoint(Uint32* pixels, int x, int y, Uint32 color, int size = 2) {
    for (int dy = -size; dy <= size; dy++) {
        for (int dx = -size; dx <= size; dx++) {
            setPixel(pixels, x + dx, y + dy, color);
        }
    }
}

// Bresenham algorithm
void drawLine(Uint32* pixels, int x0, int y0, int x1, int y1, Uint32 color) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        setPixel(pixels, x0, y0, color);

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

int main () {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("3D Rendering (in CPU!)", CANVAS_WIDTH, CANVAS_HEIGHT, 0);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // NULL to SDL choose the rendering driver
    SDL_Renderer* renderer = SDL_CreateRenderer(window, "software");
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // texture for pixel buffer
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        CANVAS_WIDTH,
        CANVAS_HEIGHT
    );
    if (texture == nullptr) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool running = true;
    SDL_Event event;
    float angle = 0.0f;
    float z_offset = 3.0f;
    Uint64 lastTime = SDL_GetTicks();

    // colors in RGBA format
    const Uint32 COLOR_BG = 0x000000FF;      // black
    const Uint32 COLOR_LINE = 0xFF0000FF;    // red
    const Uint32 COLOR_POINT = 0xFFFF00FF;   // yellow

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // delta in seconds
        lastTime = currentTime;

        angle += 2.0f * deltaTime;

        Uint32* pixels; // pixel buffer, kinda like a VRAM
        int pitch;
        SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

        // clear buffer (fill with background color)
        memset(pixels, 0, CANVAS_HEIGHT * pitch);

        // vertices processing
        std::vector<Vec2> projectedPoints;
        for (const auto& v : vertices) {
            Vec3 rotated = rotateY(v, angle);
            rotated.z += z_offset;
            Vec2 proj = project(rotated);
            projectedPoints.push_back(cartesian(proj));
        }

        // draw faces
        for(const auto& face : faces) {
            for(size_t i = 0; i < face.size(); ++i) {
                Vec2 p1 = projectedPoints[face[i]];
                Vec2 p2 = projectedPoints[face[(i + 1) % face.size()]];

                drawLine(pixels, static_cast<int>(p1.x), static_cast<int>(p1.y),
                    static_cast<int>(p2.x), static_cast<int>(p2.y), COLOR_LINE);
            }
        }

        // draw vertices
        for(const auto& p : projectedPoints) {
            drawPoint(pixels, static_cast<int>(p.x), static_cast<int>(p.y), COLOR_POINT);
        }

        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // roughly 60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}