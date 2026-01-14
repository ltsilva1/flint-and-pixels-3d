#ifndef MATH3D_H
#define MATH3D_H

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };

Vec2 cartesian(const Vec2& p, int canvasWidth, int canvasHeight);
Vec2 project(const Vec3& p);
Vec3 rotateY(const Vec3& p, float angle);

// future
// Vec3 rotateX(const Vec3& p, float angle);
// Vec3 rotateZ(const Vec3& p, float angle);

#endif