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
            std::vector<int> vIdx, tIdx, nIdx;

            // read all vertices of the face
            std::string vertexStr;
            while (ss >> vertexStr) {
                std::istringstream vss(vertexStr);
                std::string segment;

                int v = -1, t = -1, n = -1;
                if (std::getline(vss, segment, '/') && !segment.empty()) {
                    v = std::stoi(segment) - 1;
                }
                if (std::getline(vss, segment, '/') && !segment.empty()) {
                    t = std::stoi(segment) - 1;
                }
                if (std::getline(vss, segment, '/') && !segment.empty()) {
                    n = std::stoi(segment) - 1;
                }

                vIdx.push_back(v);
                tIdx.push_back(t);
                nIdx.push_back(n);
            }

            // triangulation!!
            for (size_t i = 1; i + 1 < vIdx.size(); ++i) {
                Face f;
                f.vIndices[0] = vIdx[0];
                f.vIndices[1] = vIdx[i];
                f.vIndices[2] = vIdx[i+1];

                f.tIndices[0] = tIdx[0];
                f.tIndices[1] = tIdx[i];
                f.tIndices[2] = tIdx[i+1];

                f.nIndices[0] = nIdx[0];
                f.nIndices[1] = nIdx[i];
                f.nIndices[2] = nIdx[i+1];

                mesh.faces.push_back(f);
            }
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
