#include "mesh.hpp"

#include "gl/glObject.hpp"

#include <fstream>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>

Mesh::Mesh() { }

/**
 * Reads a .obj file and populates this Mesh object.
 * Note: Assumes all "f" lines come last
 * TODO: UV Support
 **/
Mesh& Mesh::fromOBJ(std::string filename) {
    std::ifstream ifs(filename);
    if (!ifs) {
        std::cout << "File Not Found: " << filename << "\n";
        return *this;
    }

    std::vector<float> rawVertices;
    std::vector<float> rawNormals;
    // std::vector<float> rawUvs;

    std::vector<float> vertices;
    std::vector<float> normals;

    std::string type;
    std::string line;

    std::cout << "Reading Mesh File: " << filename << "\n";

    float x = 0.0f, y = 0.0f, z = 0.0f;

    while (ifs >> type) {
        if (type[0] == '#') {
            // comment, ignore the whole line
            getline(ifs, line);
        } else if (type == "v") {
            ifs >> x >> y >> z;
            rawVertices.push_back(x);
            rawVertices.push_back(y);
            rawVertices.push_back(z);
        } else if (type == "vt") {
            // ifs >> x >> y;
            // rawUvs.push_back(x);
            // rawUvs.push_back(y);

            // Skip uvs for now
            getline(ifs, line);
        } else if (type == "vn") {
            ifs >> x >> y >> z;
            rawNormals.push_back(x);
            rawNormals.push_back(y);
            rawNormals.push_back(z);
        } else if (type == "f") {
            for (uint8_t i = 0; i < 3; i++) {
                ifs >> line;
                auto s1 = line.find('/');
                auto s2 = line.find('/', s1 + 1);

                auto vtxIdx = std::stoi(line.substr(0, s1)) - 1;
                // auto uvIdx = std::stoi(line.substr(s1 + 1, s2)) - 1;
                auto nIdx = std::stoi(line.substr(s2 + 1)) - 1;

                vertices.push_back(rawVertices[vtxIdx * 3 + 0]);
                vertices.push_back(rawVertices[vtxIdx * 3 + 1]);
                vertices.push_back(rawVertices[vtxIdx * 3 + 2]);

                // uvs.push_back(rawUvs[uvIdx * 2 + 0]);
                // uvs.push_back(rawUvs[uvIdx * 2 + 1]);

                normals.push_back(rawNormals[nIdx * 3 + 0]);
                normals.push_back(rawNormals[nIdx * 3 + 1]);
                normals.push_back(rawNormals[nIdx * 3 + 2]);
            }
        } else {
            // Unknown line. Skip it
            getline(ifs, line);
        }
    }

    ifs.close();

    vertexArrayObject = std::make_shared<GLObject>(std::move(vertices), std::move(normals));

    return *this;
}
