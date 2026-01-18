#ifndef RENDERER_H
#define RENDERER_H

#include <SDL3/SDL_stdinc.h>
#include "math3d.h"

void setPixel(Uint32* pixels, int x, int y, Uint32 color);
void drawPoint(Uint32* pixels, int x, int y, Uint32 color, int size = 2);
void drawLine(Uint32* pixels, int x0, int y0, int x1, int y1, Uint32 color);
void drawTriangle(Uint32* pixels, const Vec2& v0, const Vec2& v1, const Vec2& v2, Uint32 color);

#endif
