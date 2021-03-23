#pragma once

#include "gl/glObject.hpp"

#include <memory>
#include <string>

class Mesh {
    public:
        Mesh();
        ~Mesh() = default;

        Mesh(Mesh&& other) = default;
        Mesh(const Mesh& other) = default;

        Mesh& operator=(const Mesh& other) = default;
        Mesh& operator=(Mesh&& other) = default;

        Mesh& fromOBJ(std::string filename);

        GLuint getVertexArrayObject() {
            return vertexArrayObject->getVertexArrayObject();
        }

        uint32_t getVertexCount() {
            return vertexArrayObject->getVertexCount();
        }
    private:
        // should be able to share a GLObject between different mesh entities
        std::shared_ptr<GLObject> vertexArrayObject = nullptr;
};
