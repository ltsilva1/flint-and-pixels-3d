#include <SDL3/SDL.h>
#include <iostream>
#include <cmath>
#include "config.h"
#include "renderer.h"
#include "math3d.h"
#include "mesh.h"

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

    Mesh cube = loadOBJ("starfox.obj");

    bool running = true;
    bool draw_verts = true;
    SDL_Event event;
    float angle = 0.0f;
    float z_offset = 5.5f;
    float x_offset = 0.0f;
    float y_offset = -1.0f;
    Uint64 lastTime = SDL_GetTicks();

    const bool *key_states = SDL_GetKeyboardState(nullptr);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // delta in seconds
        lastTime = currentTime;

        angle += 1.5f * deltaTime;

        Uint32* pixels; // pixel buffer, kinda like a VRAM
        int pitch;
        SDL_LockTexture(texture, nullptr, (void**)&pixels, &pitch);

        // clear buffer (fill with background color)
        memset(pixels, 0, CANVAS_HEIGHT * pitch);

        // vertices processing
        std::vector<Vec2> projectedPoints;
        for (const auto& v : cube.vertices) {
            Vec3 rotated = rotateY(v, angle);
            rotated.z += z_offset;
            rotated.x += x_offset;
            rotated.y += y_offset;
            Vec2 proj = project(rotated);
            projectedPoints.push_back(cartesian(proj, CANVAS_WIDTH, CANVAS_HEIGHT));
        }

        // draw faces
        for(const auto& face : cube.faces) {
            Vec2 p0 = projectedPoints[face.vIndices[0]];
            Vec2 p1 = projectedPoints[face.vIndices[1]];
            Vec2 p2 = projectedPoints[face.vIndices[2]];

            drawLine(pixels, static_cast<int>(p0.x), static_cast<int>(p0.y),
                static_cast<int>(p1.x), static_cast<int>(p1.y), COLOR_LINE);
            drawLine(pixels, static_cast<int>(p1.x), static_cast<int>(p1.y),
                static_cast<int>(p2.x), static_cast<int>(p2.y), COLOR_LINE);
            drawLine(pixels, static_cast<int>(p2.x), static_cast<int>(p2.y),
                static_cast<int>(p0.x), static_cast<int>(p0.y), COLOR_LINE);
        }

        // draw vertices
        for(const auto& p : projectedPoints) {
            drawPoint(pixels, static_cast<int>(p.x), static_cast<int>(p.y), COLOR_POINT);
        }


        // temp keyboard controls
        if (key_states[SDL_SCANCODE_W] || key_states[SDL_SCANCODE_UP]) {
            z_offset -= 0.1f;
        }
        if (key_states[SDL_SCANCODE_A] || key_states[SDL_SCANCODE_LEFT]) {
            x_offset += 0.1f;
        }
        if (key_states[SDL_SCANCODE_D] || key_states[SDL_SCANCODE_RIGHT]) {
            x_offset -= 0.1f;
        }
        if (key_states[SDL_SCANCODE_S] || key_states[SDL_SCANCODE_DOWN]) {
            z_offset += 0.1f;
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
