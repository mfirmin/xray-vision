#include "stencilMaterial.hpp"

#include "gl/shaderUtils.hpp"

#include "light/light.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>

StencilMaterial::StencilMaterial() : Material(glm::vec3(0.0, 0.0, 0.0), 0.0, 0.0) {
}

void StencilMaterial::create() {
    if (getProgram() != 0) {
        // already initialized
        return;
    }
    std::string vertexShaderSource = R"(
        #version 330
        layout(location = 0) in vec3 position;
        uniform mat4 projectionMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 modelMatrix;

        void main() {
            gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330

        out vec4 fragColor;

        void main() {
            fragColor = vec4(1.0, 0.0, 1.0, 1.0);
        }
    )";

    if (!compile(vertexShaderSource, fragmentShaderSource)) {
        return;
    }

    GLuint shader = getProgram();

    glUseProgram(shader);
    auto projectionMatrixLocation = glGetUniformLocation(shader, "projectionMatrix");
    auto viewMatrixLocation = glGetUniformLocation(shader, "viewMatrix");
    auto modelMatrixLocation = glGetUniformLocation(shader, "modelMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUseProgram(0);
}

StencilMaterial::~StencilMaterial() {}
