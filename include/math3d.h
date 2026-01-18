#ifndef MATH3D_H
#define MATH3D_H

struct Vec3 { float x, y, z; };
struct Vec2 { float x, y; };

struct Triangle { Vec3 v[3]; }; // move this?

Vec2 cartesian(const Vec2& p, int canvasWidth, int canvasHeight);
Vec2 project(const Vec3& p);
Vec3 rotateY(const Vec3& p, float angle);
Vec3 intersectPlane(const Vec3& inside, const Vec3& outside, float z_plane);

// future
// Vec3 rotateX(const Vec3& p, float angle);
// Vec3 rotateZ(const Vec3& p, float angle);

#endif
