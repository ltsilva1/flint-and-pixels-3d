#include "math3d.h"
#include <cmath>

Vec2 cartesian(const Vec2& p, int canvasWidth, int canvasHeight) {
    const float nx  = (p.x + 1) / 2;
    const float ny = 1.0f - (p.y + 1) / 2; // reverse y-axis because conventions and all

    return {nx * canvasWidth, ny * canvasHeight};
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
