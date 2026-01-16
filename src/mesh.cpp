#include "mesh.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "math3d.h"


Mesh loadOBJ(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            Vec3 v;
            ss >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v);
        } else if (prefix == "vt") {
            Vec2 vt;
            ss >> vt.x >> vt.y;
            // future: maybe invert V if textures are upside down (vt.y = 1.0f - vt.y);
            mesh.uvs.push_back(vt);
        } else if (prefix == "vn") {
            Vec3 vn;
            ss >> vn.x >> vn.y >> vn.z;
            mesh.normals.push_back(vn);
        } else if (prefix == "f") {
            Face f;

            // security initialization
            for(int k=0; k<3; k++) {
                f.vIndices[k] = -1;
                f.tIndices[k] = -1;
                f.nIndices[k] = -1;
            }

            for (int i = 0; i < 3; ++i) {
                std::string vertexStr;
                ss >> vertexStr;

                std::istringstream vss(vertexStr);
                std::string segment;

                if (std::getline(vss, segment, '/')) {
                    if (!segment.empty()) {
                        f.vIndices[i] = std::stoi(segment) - 1; // OBJ index correction
                    }
                }

                if (std::getline(vss, segment, '/')) {
                    if (!segment.empty()) {
                        f.tIndices[i] = std::stoi(segment) - 1;
                    }
                }

                if (std::getline(vss, segment, '/')) {
                    if (!segment.empty()) {
                        f.nIndices[i] = std::stoi(segment) - 1;
                    }
                }
            }

            mesh.faces.push_back(f);
        }
    }
    return mesh;
}

// hardcoded cube for first testing
Mesh createExampleCube() {
    Mesh mesh;
    const float lado = 0.5f;

    mesh.vertices = {
        {lado, lado, lado},
        {-lado, lado, lado},
        {-lado, -lado, lado},
        {lado, -lado, lado},

        {lado, lado, -lado},
        {-lado, lado, -lado},
        {-lado, -lado, -lado},
        {lado, -lado, -lado}
    };

    std::vector<std::vector<int>> quads = {
        {0, 1, 2, 3},
        {5, 4, 7, 6},
        {4, 0, 3, 7},
        {1, 5, 6, 2},
        {4, 5, 1, 0},
        {3, 2, 6, 7}
    };

    // triangulation!!!
    for (const auto& q : quads) {
        // {a, b, c, d} -> {a, b, c} and {a, c, d}

        Face t1;
        t1.vIndices[0] = q[0];
        t1.vIndices[1] = q[1];
        t1.vIndices[2] = q[2];

        Face t2;
        t2.vIndices[0] = q[0];
        t2.vIndices[1] = q[2];
        t2.vIndices[2] = q[3];

        for(int i=0; i<3; i++) {
            t1.tIndices[i] = t1.nIndices[i] = -1;
            t2.tIndices[i] = t2.nIndices[i] = -1;
        }

        mesh.faces.push_back(t1);
        mesh.faces.push_back(t2);
    }

    return mesh;
}
