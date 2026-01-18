#include <SDL3/SDL.h>
#include <SDL3/SDL_scancode.h>
#include <asm-generic/errno.h>
#include <iostream>
#include <cmath>
#include "config.h"
#include "renderer.h"
#include "math3d.h"
#include "mesh.h"

int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <mesh_file>" << std::endl;
        return 1;
    }

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("3D Rendering in CPU", CANVAS_WIDTH, CANVAS_HEIGHT, 0);
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

    Mesh cube = loadOBJ(argv[1]);
    //Mesh cube = createExampleCube();

    bool running = true;
    bool rotate = true;
    bool draw_verts = true;
    SDL_Event event;
    float angle = 0.0f;
    float z_offset = 15.0f;
    float x_offset = 0.0f;
    float y_offset = -1.0f;
    Uint64 lastTime = SDL_GetTicks();

    const bool *key_states = SDL_GetKeyboardState(nullptr);

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                switch (event.key.scancode) { // temp input
                    case SDL_SCANCODE_W:
                        z_offset -= 1.0f;
                        break;
                    case SDL_SCANCODE_S:
                        z_offset += 1.0f;
                        break;
                    case SDL_SCANCODE_A:
                        x_offset += 1.0f;
                        break;
                    case SDL_SCANCODE_D:
                        x_offset -= 1.0f;
                        break;
                    case SDL_SCANCODE_Q:
                        y_offset += 1.0f;
                        break;
                    case SDL_SCANCODE_E:
                        y_offset -= 1.0f;
                        break;
                    case SDL_SCANCODE_V:
                        draw_verts = !draw_verts;
                        break;
                    case SDL_SCANCODE_R:
                        rotate = !rotate;
                        break;
                }
            }
        }

        Uint64 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f; // delta in seconds
        lastTime = currentTime;

        if (rotate) {
            angle += 1.5f * deltaTime;
        }

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
        for (const auto& face : cube.faces) {
            Vec3 v0 = cube.vertices[face.vIndices[0]];
            Vec3 v1 = cube.vertices[face.vIndices[1]];
            Vec3 v2 = cube.vertices[face.vIndices[2]];

            // TODO: clean up this mess
            // transform for view space stuff
            Vec3 t0 = rotateY(v0, angle);
            t0.z += z_offset;
            t0.x += x_offset;
            t0.y += y_offset;
            Vec3 t1 = rotateY(v1, angle);
            t1.z += z_offset;
            t1.x += x_offset;
            t1.y += y_offset;
            Vec3 t2 = rotateY(v2, angle);
            t2.z += z_offset;
            t2.x += x_offset;
            t2.y += y_offset;

            // 3D CLIPPING!!
            Triangle triView = { t0, t1, t2 };
            std::vector<Triangle> clipped = clipTriangleAgainstNear(triView);

            // project and draw triangles
            for (const auto& tri : clipped) {
                Vec2 p0 = project(tri.v[0]);
                Vec2 p1 = project(tri.v[1]);
                Vec2 p2 = project(tri.v[2]);

                p0 = cartesian(p0, CANVAS_WIDTH, CANVAS_HEIGHT);
                p1 = cartesian(p1, CANVAS_WIDTH, CANVAS_HEIGHT);
                p2 = cartesian(p2, CANVAS_WIDTH, CANVAS_HEIGHT);

                drawTriangle(pixels, p0, p1, p2, COLOR_LINE);
            }
        }


        // draw vertices
        if(draw_verts)
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
