#include "material.hpp"

#include "gl/shaderUtils.hpp"

#include "light/light.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <string>
#include <sstream>

Material::Material(glm::vec3 color, float specularCoefficient, float shininess) :
    color(color),
    specularCoefficient(specularCoefficient),
    shininess(shininess)
{}

void Material::create() {
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

        #define MAX_LIGHTS 10

        out vec4 fColor;

        uniform mat4 viewMatrix;

        uniform vec3 color;
        uniform float shininess;

        uniform vec3 emissiveColor;
        uniform float emissiveStrength;
        uniform float emissiveEnabled;

        uniform float blinnEnabled;

        uniform float specularCoefficient;

        in vec3 vNormalEyespace;
        in vec4 vPositionEyespace;

        uniform int numLights;
        uniform struct Light {
            vec4 position;
            vec3 color;
            float intensity;
            float ambientCoefficient;
            float attenuation;
            float enabled;
            // spotlight only
            float coneAngle;
            vec3 coneDirection;
        } lights[MAX_LIGHTS];

        vec3 illuminate(vec3 inColor, vec3 P, vec3 N, vec3 E) {
            vec3 outColor = vec3(0.0);

            for (int i = 0; i < numLights; i++) {
                Light light = lights[i];
                if (light.enabled < 0.5) {
                    continue;
                }
                vec3 L;
                float attenuation;
                if (light.position.w == 0.0) {
                    // directional light
                    // convert the direction into eyespace
                    L = normalize((viewMatrix * light.position).xyz);
                    // directional lights don't support attenuation
                    attenuation = 1.0;
                } else {
                    vec3 lightPositionEyespace = (viewMatrix * light.position).xyz;
                    L = normalize(lightPositionEyespace - P);
                    float distance = length(lightPositionEyespace - P);
                    attenuation = 1.0 / (1.0 + light.attenuation * pow(distance, 2));
                }
                // TODO: Spotlights

                vec3 H = normalize(L + E);

                vec3 ambient = light.ambientCoefficient * inColor * light.color * light.intensity;

                float diffuseCoefficient = max(0.0, dot(N, L));
                vec3 diffuse = diffuseCoefficient * inColor * light.color * light.intensity;

                float specularTerm = 0.0;

                if (diffuseCoefficient > 0.0) {
                    float dir = 0.0;
                    if (blinnEnabled > 0.5f) {
                        dir = dot(N, H);
                    } else {
                        dir = dot(
                            E,
                            reflect(-L, N)
                        );
                    }
                    specularTerm = pow(
                        max(
                            0.0,
                            dir
                        ),
                        shininess
                    );
                }

                vec3 specular = specularCoefficient * specularTerm * inColor * light.color * light.intensity;

                // TODO: Shadows

                outColor += ambient + attenuation * (diffuse + specular);
            }

            // TODO: Gamma correction
            if (emissiveEnabled > 0.5f) {
                outColor += emissiveStrength * emissiveColor;
            }
            return outColor;
        }

        void main() {
            vec3 N = normalize(vNormalEyespace);
            vec3 E = normalize(-vPositionEyespace.xyz);

            if (dot(N, E) < 0.0) {
                N = -N;
            }

            fColor = vec4(illuminate(color, vPositionEyespace.xyz, N, E), 1.0);
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
    auto shininessLocation = glGetUniformLocation(shader, "shininess");
    auto specularCoefficientLocation = glGetUniformLocation(shader, "specularCoefficient");
    auto emissiveColorLocation = glGetUniformLocation(shader, "emissiveColor");
    auto emissiveStrengthLocation = glGetUniformLocation(shader, "emissiveStrength");
    auto emissiveEnabledLocation = glGetUniformLocation(shader, "emissiveEnabled");
    auto blinnEnabledLocation = glGetUniformLocation(shader, "blinnEnabled");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glUniform1f(shininessLocation, shininess);
    glUniform1f(specularCoefficientLocation, specularCoefficient);
    glUniform3fv(emissiveColorLocation, 1, glm::value_ptr(color));
    glUniform1f(emissiveStrengthLocation, 0.0f);
    glUniform1f(emissiveEnabledLocation, 0.0f);
    glUniform1f(blinnEnabledLocation, 1.0f);
    glUseProgram(0);
}

Material::~Material() {}

bool Material::compile(std::string vertexShader, std::string fragmentShader) {
    program = ShaderUtils::compile(vertexShader, fragmentShader);

    return program != 0;
}

void Material::setColor(glm::vec3 color) {
    glUseProgram(program);
    auto colorLocation = glGetUniformLocation(program, "color");
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glUseProgram(0);
}

void Material::setEmissiveColorAndStrength(glm::vec3 color, float strength) {
    glUseProgram(program);
    auto emissiveColorLocation = glGetUniformLocation(program, "emissiveColor");
    auto emissiveStrengthLocation = glGetUniformLocation(program, "emissiveStrength");
    glUniform3fv(emissiveColorLocation, 1, glm::value_ptr(color));
    glUniform1f(emissiveStrengthLocation, strength);
    glUseProgram(0);
}

void Material::setEmissiveColor(glm::vec3 color) {
    glUseProgram(program);
    auto emissiveColorLocation = glGetUniformLocation(program, "emissiveColor");
    glUniform3fv(emissiveColorLocation, 1, glm::value_ptr(color));
    glUseProgram(0);
}
void Material::setEmissiveStrength(float strength) {
    glUseProgram(program);
    auto emissiveStrengthLocation = glGetUniformLocation(program, "emissiveStrength");
    glUniform1f(emissiveStrengthLocation, strength);
    glUseProgram(0);
}

void Material::toggleEmissive(bool value) {
    glUseProgram(program);
    auto emissiveEnabledLocation = glGetUniformLocation(program, "emissiveEnabled");
    glUniform1f(emissiveEnabledLocation, value ? 1.0f : 0.0f);
    glUseProgram(0);
}

void Material::toggleBlinnPhongShading(bool value) {
    glUseProgram(program);
    auto blinnEnabledLocation = glGetUniformLocation(program, "blinnEnabled");
    glUniform1f(blinnEnabledLocation, value ? 1.0f : 0.0f);
    glUseProgram(0);
}

void Material::setShininess(float shininess) {
    glUseProgram(program);
    auto shininessLocation = glGetUniformLocation(program, "shininess");
    glUniform1f(shininessLocation, shininess);
    glUseProgram(0);
}

void Material::setLights(const std::vector<std::shared_ptr<Light>>& lights) {
    std::size_t lightIndex = 0;

    glUseProgram(program);

    glUniform1i(glGetUniformLocation(program, "numLights"), lights.size());

    for (const auto& light : lights) {
        auto lightInfo = light->getLightInfo();

        // position
        std::ostringstream positionLocation;
        std::ostringstream colorLocation;
        std::ostringstream intensityLocation;
        std::ostringstream attenuationLocation;
        std::ostringstream ambientCoefficientLocation;
        std::ostringstream enabledLocation;
        std::ostringstream coneAngleLocation;
        std::ostringstream coneDirectionLocation;

        positionLocation << "lights[" << lightIndex << "].position";
        colorLocation << "lights[" << lightIndex << "].color";
        intensityLocation << "lights[" << lightIndex << "].intensity";
        attenuationLocation << "lights[" << lightIndex << "].attenuation";
        ambientCoefficientLocation << "lights[" << lightIndex << "].ambientCoefficient";
        enabledLocation << "lights[" << lightIndex << "].enabled";
        coneAngleLocation << "lights[" << lightIndex << "].coneAngle";
        coneDirectionLocation << "lights[" << lightIndex << "].coneDirection";

        glUniform4fv(
            glGetUniformLocation(program, positionLocation.str().c_str()),
            1,
            glm::value_ptr(lightInfo.position)
        );

        glUniform3fv(
            glGetUniformLocation(program, colorLocation.str().c_str()),
            1,
            glm::value_ptr(lightInfo.color)
        );

        glUniform1f(
            glGetUniformLocation(program, intensityLocation.str().c_str()),
            lightInfo.intensity
        );

        glUniform1f(
            glGetUniformLocation(program, attenuationLocation.str().c_str()),
            lightInfo.attenuation
        );

        glUniform1f(
            glGetUniformLocation(program, ambientCoefficientLocation.str().c_str()),
            lightInfo.ambientCoefficient
        );

        glUniform1f(
            glGetUniformLocation(program, enabledLocation.str().c_str()),
            lightInfo.enabled ? 1.0f : 0.0f
        );

        glUniform1f(
            glGetUniformLocation(program, coneAngleLocation.str().c_str()),
            lightInfo.coneAngle
        );

        glUniform3fv(
            glGetUniformLocation(program, coneDirectionLocation.str().c_str()),
            1,
            glm::value_ptr(lightInfo.coneDirection)
        );


        lightIndex++;
    }

    glUseProgram(0);
}

void Material::setModelMatrix(const glm::mat4& modelMatrix) {
    glUseProgram(program);
    auto modelMatrixLocation = glGetUniformLocation(program, "modelMatrix");
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUseProgram(0);
}

void Material::setProjectionAndViewMatrices(
    const glm::mat4& projectionMatrix,
    const glm::mat4& viewMatrix
) {
    glUseProgram(program);
    auto projectionMatrixLocation = glGetUniformLocation(program, "projectionMatrix");
    auto viewMatrixLocation = glGetUniformLocation(program, "viewMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUseProgram(0);
}

void Material::setMatrices(
    const glm::mat4& projectionMatrix,
    const glm::mat4& viewMatrix,
    const glm::mat4& modelMatrix
) {
    glUseProgram(program);
    auto projectionMatrixLocation = glGetUniformLocation(program, "projectionMatrix");
    auto viewMatrixLocation = glGetUniformLocation(program, "viewMatrix");
    auto modelMatrixLocation = glGetUniformLocation(program, "modelMatrix");
    glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
    glUseProgram(0);
}
