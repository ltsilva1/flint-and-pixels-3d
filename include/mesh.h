#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include "math3d.h"

// OBJ structure
struct Face {
    int vIndices[3];
    int tIndices[3];
    int nIndices[3];
    // fixed 3 positions array to force triangle mesh
};

struct Mesh {
    std::vector<Vec3> vertices; // v
    std::vector<Vec2> uvs;      // vt
    std::vector<Vec3> normals;  // vn
    std::vector<Face> faces;    // f
};

Mesh loadOBJ(const std::string& filename);
Mesh createExampleCube();
std::vector<Triangle> clipTriangleAgainstNear(const Triangle& in_tri);

#endif // MESH_H
