#include "auraMaterial.hpp"

#include "gl/shaderUtils.hpp"

#include "light/light.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>

AuraMaterial::AuraMaterial(glm::vec3 color) : Material(glm::vec3(0.0, 0.0, 0.0), 0.0, 0.0), color(color) {
}

void AuraMaterial::create() {
    if (getProgram() != 0) {
        // already initialized
        return;
    }
    std::string vertexShaderSource = R"(
        #version 330
        layout(location = 0) in vec3 position;
        layout(location = 1) in vec3 normal;

        uniform mat4 projectionMatrix;
        uniform mat4 viewMatrix;
        uniform mat4 modelMatrix;

        out vec3 vNormalEyespace;

        void main() {
            vNormalEyespace = (transpose(inverse(viewMatrix * modelMatrix)) * vec4(normal, 0.0)).xyz;
            gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330

        out vec4 fragColor;

        in vec3 vNormalEyespace;

        uniform vec3 color;

        void main() {
            float alpha = 1.0 - dot(vNormalEyespace, vec3(0.0, 0.0, 1.0));
            fragColor = vec4(color, alpha);
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
    auto colorLocation = glGetUniformLocation(shader, "color");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glUseProgram(0);
}

AuraMaterial::~AuraMaterial() {}
