#include "renderer.h"
#include <cmath>
#include "config.h"

// USE OOP HERE

void setPixel(Uint32* pixels, int x, int y, Uint32 color) {
    if (x >= 0 && x < CANVAS_WIDTH && y >= 0 && y < CANVAS_HEIGHT) { // kinda slow?
        pixels[y * CANVAS_WIDTH + x] = color;
    }
}

void drawPoint(Uint32* pixels, int x, int y, Uint32 color, int size) {
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