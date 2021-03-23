#include "deferredMaterial.hpp"

#include "gl/shaderUtils.hpp"

#include "light/light.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <sstream>

DeferredMaterial::DeferredMaterial(glm::vec3 color, float specularCoefficient, float shininess) : Material(color, specularCoefficient, shininess) {
}

void DeferredMaterial::create() {
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
        out vec4 vPositionEyespace;

        void main() {
            vNormalEyespace = (transpose(inverse(viewMatrix * modelMatrix)) * vec4(normal, 0.0)).xyz;
            vPositionEyespace = viewMatrix * modelMatrix * vec4(position, 1.0);

            gl_Position = projectionMatrix * vPositionEyespace;
        }
    )";

    std::string fragmentShaderSource = R"(
        #version 330

        layout(location = 0) out vec4 position;
        layout(location = 1) out vec3 normal;
        layout(location = 2) out vec4 albedo;
        layout(location = 3) out vec4 emissive;

        uniform mat4 viewMatrix;

        uniform vec3 color;
        uniform float specularCoefficient;

        // todo: shininess?
        // uniform float shininess;

        uniform vec3 emissiveColor;
        uniform float emissiveStrength;
        uniform float emissiveEnabled;

        in vec3 vNormalEyespace;
        in vec4 vPositionEyespace;

        void main() {
            vec3 N = normalize(vNormalEyespace);
            vec3 E = normalize(-vPositionEyespace.xyz);

            if (dot(N, E) < 0.0) {
                N = -N;
            }

            position = vPositionEyespace;
            normal = N;
            albedo = vec4(color, specularCoefficient);

            if (emissiveEnabled > 0.5) {
                emissive = vec4(emissiveColor, emissiveStrength);
            }
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
    auto specularCoefficientLocation = glGetUniformLocation(shader, "specularCoefficient");
    auto emissiveColorLocation = glGetUniformLocation(shader, "emissiveColor");
    auto emissiveStrengthLocation = glGetUniformLocation(shader, "emissiveStrength");
    auto emissiveEnabledLocation = glGetUniformLocation(shader, "emissiveEnabled");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniform3fv(colorLocation, 1, glm::value_ptr(getColor()));
    glUniform1f(specularCoefficientLocation, getSpecularCoefficient());
    glUniform3fv(emissiveColorLocation, 1, glm::value_ptr(getColor()));
    glUniform1f(emissiveStrengthLocation, 0.0f);
    glUniform1f(emissiveEnabledLocation, 0.0f);
    glUseProgram(0);
}

DeferredMaterial::~DeferredMaterial() {}
