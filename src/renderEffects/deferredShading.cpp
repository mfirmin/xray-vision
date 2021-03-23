#include "deferredShading.hpp"

#include "gl/shaderUtils.hpp"
#include "light/light.hpp"

#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <sstream>

DeferredShadingEffect::DeferredShadingEffect(int w, int h) :
    width(w), height(h)
{
}

void DeferredShadingEffect::initialize() {
    // initialize the framebuffer for the render target
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // initialize the texture to bind to the buffer
    glGenTextures(1, &positionTexture);
    glBindTexture(GL_TEXTURE_2D, positionTexture);

    // floating point texture, RGB for position, A for depth
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, positionTexture, 0);

    glGenTextures(1, &normalTexture);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    // floating point texture, RGB for position, A for depth
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, normalTexture, 0);

    glGenTextures(1, &albedoTexture);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);

    // floating point texture, RGB for position, A for depth
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, albedoTexture, 0);

    glGenTextures(1, &emissiveTexture);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);

    // floating point texture, RGB for position, A for depth
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, emissiveTexture, 0);

    // initialize the depth buffer
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    // attach the depth buffer to the frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    std::array<GLenum, 4> drawbuffers = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };


    glDrawBuffers(4, drawbuffers.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating DeferredShadingEffect: Error creating framebuffer\n";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    createOutput();
    createDebugProgram();
    createProgram();
}

DeferredShadingEffect::~DeferredShadingEffect() {
    // TODO: Free buffers
}

void DeferredShadingEffect::createOutput() {
    glGenFramebuffers(1, &outputFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);

    // initialize the texture to bind to the buffer
    glGenTextures(1, &outputTexture);
    glBindTexture(GL_TEXTURE_2D, outputTexture);

    // floating point texture, RGB for position, A for depth
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTexture, 0);

    std::array<GLenum, 1> drawbuffers = { GL_COLOR_ATTACHMENT0 };


    glDrawBuffers(1, drawbuffers.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating DeferredShadingEffect: Error creating output framebuffer\n";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DeferredShadingEffect::createDebugProgram() {
    std::string vertexShaderSource = R"(
        #version 330
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 uv;

        out vec2 vUv;

        void main() {
            vUv = uv;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )";


    std::string fragmentShaderSource = R"(
        #version 330

        // scene is a floating point (HDR) texture
        uniform sampler2D input;

        in vec2 vUv;

        out vec4 fragColor;

        void main() {
            vec3 color = texture(input, vUv).rgb;

            color = color + vec3(1.0) * 0.5;

            fragColor = vec4(color, 1.0);
        }
    )";

    debugProgram = ShaderUtils::compile(vertexShaderSource, fragmentShaderSource);
}

void DeferredShadingEffect::createProgram() {
    std::string vertexShaderSource = R"(
        #version 330
        layout(location = 0) in vec2 position;
        layout(location = 1) in vec2 uv;

        out vec2 vUv;

        void main() {
            vUv = uv;
            gl_Position = vec4(position, 0.0, 1.0);
        }
    )";


    std::string fragmentShaderSource = R"(
        #version 330

        #define MAX_LIGHTS 10

        uniform mat4 viewMatrix;

        uniform float blinnEnabled;
        uniform float emissiveEnabled;
        uniform float ssaoEnabled;

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

        // scene is a floating point (HDR) texture
        uniform sampler2D gPosition;
        uniform sampler2D gNormal;
        uniform sampler2D gAlbedo;
        uniform sampler2D gEmissive;
        uniform sampler2D ambientOcclusion;

        in vec2 vUv;

        out vec4 fragColor;

        vec3 illuminate(vec4 albedo, vec4 emissive, vec3 P, vec3 N, vec3 E) {
            vec3 inColor = albedo.rgb;
            vec3 emissiveColor = emissive.rgb;
            float emissiveStrength = emissive.a;

            float specularCoefficient = albedo.a;

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

                float ao = 1.0;

                if (ssaoEnabled > 0.5f) {
                    ao = texture(ambientOcclusion, vUv).r;
                }

                vec3 ambient = light.ambientCoefficient * inColor * light.color * light.intensity * ao;

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
                        32.0
                    );
                }

                // Specular color can (and should) be a different color than diffuse
                // this is because it often represents a top glossy layer over the actual paint of the object.

                vec3 specularColor = vec3(1.0, 1.0, 1.0);

                vec3 specular = specularCoefficient * specularTerm * specularColor * light.color * light.intensity;

                // TODO: Shadows

                outColor += ambient + attenuation * (diffuse + specular);
            }

            if (emissiveEnabled > 0.5f) {
                outColor += emissiveStrength * emissiveColor;
            }

            return outColor;
        }

        void main() {
            vec4 p = texture(gPosition, vUv);
            vec3 position = p.xyz;

            vec3 normal = texture(gNormal, vUv).rgb;
            vec4 albedo = texture(gAlbedo, vUv);
            vec4 emissive = texture(gEmissive, vUv);

            vec3 N = normalize(normal);
            vec3 E = normalize(-position);

            if (dot(N, E) < 0.0) {
                N = -N;
            }

            vec3 color = albedo.rgb;

            if (p.w > 0.5f) {
                color = illuminate(albedo, emissive, position, N, E);
            }

            fragColor = vec4(color, 1.0);
        }
    )";

    program = ShaderUtils::compile(vertexShaderSource, fragmentShaderSource);

    glUseProgram(program);
    glUniform1f(glGetUniformLocation(program, "blinnEnabled"), 1.0f);
    glUniform1f(glGetUniformLocation(program, "ssaoEnabled"), 1.0f);
    glUniform1f(glGetUniformLocation(program, "emissiveEnabled"), 1.0f);
    glUseProgram(0);
}

void DeferredShadingEffect::setLights(const std::vector<std::shared_ptr<Light>>& lights) {
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

void DeferredShadingEffect::setViewMatrix(const glm::mat4& viewMatrix) {
    glUseProgram(program);
    auto viewMatrixLocation = glGetUniformLocation(program, "viewMatrix");
    glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUseProgram(0);
}

void DeferredShadingEffect::toggleBlinnPhongShading(bool value) {
    glUseProgram(program);
    auto blinnEnabledLocation = glGetUniformLocation(program, "blinnEnabled");
    glUniform1f(blinnEnabledLocation, value ? 1.0f : 0.0f);
    glUseProgram(0);
}

void DeferredShadingEffect::toggleSSAO(bool value) {
    glUseProgram(program);
    auto ssaoEnabledLocation = glGetUniformLocation(program, "ssaoEnabled");
    glUniform1f(ssaoEnabledLocation, value ? 1.0f : 0.0f);
    glUseProgram(0);
}

void DeferredShadingEffect::toggleIBL(bool value) {
    (void)value;
}

void DeferredShadingEffect::render(GLuint vao, GLuint ambientOcclusion) {
    glBindFramebuffer(GL_FRAMEBUFFER, outputFbo);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Clear it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render the screen object to it
    glBindVertexArray(vao);
    auto deferredProgram = program;
    // use the debug program from the deferred target (just render 1 property)
    glUseProgram(deferredProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, positionTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTexture);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, albedoTexture);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, ambientOcclusion);

    glUniform1i(glGetUniformLocation(deferredProgram, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(deferredProgram, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(deferredProgram, "gAlbedo"), 2);
    glUniform1i(glGetUniformLocation(deferredProgram, "gEmissive"), 3);
    glUniform1i(glGetUniformLocation(deferredProgram, "ambientOcclusion"), 4);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
}
