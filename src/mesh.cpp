#include "mesh.h"

#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "math3d.h"
#include "config.h"


Mesh loadOBJ(const std::string& filename) {
    Mesh mesh;
    std::ifstream file(filename);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

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

std::vector<Triangle> clipTriangleAgainstNear(const Triangle& in_tri) {
    std::vector<Triangle> out_tris;

    // which vertex is inside or outside
    Vec3 inside_pts[3];
    Vec3 outside_pts[3];
    int in_count = 0;
    int out_count = 0;

    for (int i = 0; i < 3; i++) {
        if (in_tri.v[i].z >= Z_NEAR) {
            inside_pts[in_count++] = in_tri.v[i];
        } else {
            outside_pts[out_count++] = in_tri.v[i];
        }
    }

    // all out = reject the triangle
    if (in_count == 0) {
        return out_tris;
    }
    // all in = keep the triangle
    else if (in_count == 3) {
        out_tris.push_back(in_tri);
    }
    // 1 in 2 out = become one smaller triangle
    else if (in_count == 1) {
        Triangle t;
        t.v[0] = inside_pts[0];
        t.v[1] = intersectPlane(inside_pts[0], outside_pts[0], Z_NEAR);
        t.v[2] = intersectPlane(inside_pts[0], outside_pts[1], Z_NEAR);
        out_tris.push_back(t);
    }
    // 2 in 1 out = quad = 2 tris
    else if (in_count == 2) {
        // find outside index to ((preserve)) winding order
        int idx_out;
        if (in_tri.v[0].z < Z_NEAR) idx_out = 0;
        else if (in_tri.v[1].z < Z_NEAR) idx_out = 1;
        else idx_out = 2;

        int idx_in1 = (idx_out + 1) % 3;
        int idx_in2 = (idx_out + 2) % 3;

        // get vertices
        Vec3 v_out = in_tri.v[idx_out];
        Vec3 v_in1 = in_tri.v[idx_in1];
        Vec3 v_in2 = in_tri.v[idx_in2];

        // calculate intersection points
        Vec3 p1 = intersectPlane(v_in1, v_out, Z_NEAR);
        Vec3 p2 = intersectPlane(v_in2, v_out, Z_NEAR);

        Triangle t1;
        t1.v[0] = v_in1;
        t1.v[1] = p1;
        t1.v[2] = v_in2;
        out_tris.push_back(t1);

        Triangle t2;
        t2.v[0] = p1;
        t2.v[1] = p2;
        t2.v[2] = v_in2;
        out_tris.push_back(t2);
    }

    return out_tris;
}
