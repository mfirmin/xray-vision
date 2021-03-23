#include "glObject.hpp"

#include <iostream>

GLObject::GLObject() {
    createVertexArrayObject();
}

GLObject::GLObject(
    std::vector<float>&& vs,
    std::vector<float>&& ns
) {
    createVertexArrayObject();
    setVertices(std::move(vs));
    setNormals(std::move(ns));
}

void GLObject::createVertexArrayObject() {
    glGenVertexArrays(1, &vertexArrayObject);
}

void GLObject::setVertices(std::vector<float>&& vs) {
    vertices.clear();
    for (auto vtx : vs) {
        vertices.push_back(vtx);
    }
    vertexCount = vertices.size() / 3;

    if (vertexBuffer == 0) {
        glGenBuffers(1, &vertexBuffer);

        glBindVertexArray(vertexArrayObject);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);
}

void GLObject::setNormals(std::vector<float>&& ns) {
    normals = std::move(ns);

    if (normalBuffer == 0) {
        glGenBuffers(1, &normalBuffer);

        glBindVertexArray(vertexArrayObject);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    }

    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(GL_FLOAT), normals.data(), GL_STATIC_DRAW);
}
