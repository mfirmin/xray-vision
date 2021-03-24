#include "auraCompositing.hpp"

#include "gl/shaderUtils.hpp"
#include "light/light.hpp"

#include <array>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <sstream>

AuraCompositingEffect::AuraCompositingEffect(int w, int h) :
    width(w), height(h)
{
}

void AuraCompositingEffect::initialize() {
    createDebugProgram();
    createProgram();
    createArrayObjects();
}

AuraCompositingEffect::~AuraCompositingEffect() {
    // TODO: Free buffers
}

void AuraCompositingEffect::createArrayObjects() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vertexBuffer);
    glGenBuffers(1, &uvBuffer);

    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // NDC Coords
    std::vector<float> vertices = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        -1.0f, 1.0f,
        1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    std::vector<float> uvs = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GL_FLOAT), uvs.data(), GL_STATIC_DRAW);
}

void AuraCompositingEffect::createDebugProgram() {
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

void AuraCompositingEffect::createProgram() {
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

        uniform mat4 viewMatrix;

        uniform sampler2D scene;
        uniform sampler2D aura;

        in vec2 vUv;

        out vec4 fragColor;

        void main() {
            vec4 sceneColor = texture(scene, vUv);
            vec4 auraColor = texture(aura, vUv);

            fragColor = vec4(mix(sceneColor.rgb, auraColor.rgb, auraColor.a), 1.0);
        }
    )";

    program = ShaderUtils::compile(vertexShaderSource, fragmentShaderSource);
}


void AuraCompositingEffect::render(GLuint sceneTexture, GLuint auraTexture) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // Clear it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render the screen object to it
    glBindVertexArray(vao);

    auto prog = program;
    // use the debug program from the deferred target (just render 1 property)
    glUseProgram(prog);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, auraTexture);

    glUniform1i(glGetUniformLocation(prog, "scene"), 0);
    glUniform1i(glGetUniformLocation(prog, "aura"), 1);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
}
