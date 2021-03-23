#include "renderer.hpp"

#include "camera.hpp"
#include "gl/shaderUtils.hpp"
#include "light/light.hpp"
#include "material/material.hpp"
#include "model.hpp"
#include "renderTarget.hpp"

#include <GL/glew.h>

#include <iostream>

constexpr GLuint GL_MAJOR = 3;
constexpr GLuint GL_MINOR = 3;

Renderer::Renderer(int width, int height, std::unique_ptr<Camera>&& camera) :
    width(width),
    height(height),
    camera(std::move(camera))
{
    std::cout << "Initializing SDL...\n";
    if (!initializeSDL()) {
        std::cout << "Failed to initialize SDL\n";
        return;
    }

    std::cout << "Initializing GL...\n";
    if (!initializeGL()) {
        std::cout << "Failed to initialize GL\n";
        return;
    }

    initializeScreenObject();

    std::cout << "Ready\n";
}

Renderer::~Renderer() {
    SDL_DestroyWindow(window);

    window = nullptr;
    SDL_Quit();
}

bool Renderer::initializeSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized\n";
    } else {
        window = SDL_CreateWindow("Model Viewer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
        if (window == nullptr) {
            std::cout << "Window could not be created!\n";
        } else {
            screen = SDL_GetWindowSurface(window);
        }
    }

    return true;
}

bool Renderer::initializeGL() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, GL_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, GL_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(window);

    if (context == nullptr) {
        std::cout << "Error creating openGL context: " << SDL_GetError() << "\n";
        return false;
    }

    glewExperimental = GL_TRUE;
    GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cout << "Error initializing GLEW\n";
        return false;
    }

    if (SDL_GL_SetSwapInterval(1) < 0) {
        std::cout << "Unable to set VSync\n";
        return false;
    }

    glEnable(GL_PROGRAM_POINT_SIZE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Enable writing to depth buffer
    glDepthMask(GL_TRUE);

    // necessary for cubemaps to work correctly
    // glDepthFunc(GL_LEQUAL);
    // Enable MultiSampling
    // glEnable(GL_MULTISAMPLE);
    // Enable face culling
    glEnable(GL_CULL_FACE);

    return true;
}

void Renderer::initializeScreenObject() {
    // glGenVertexArrays(1, &screenObject.vertexArray);
    // glGenBuffers(1, &screenObject.vertexBuffer);
    // glGenBuffers(1, &screenObject.uvBuffer);

    // glBindVertexArray(screenObject.vertexArray);

    // glEnableVertexAttribArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, screenObject.vertexBuffer);
    // glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // // NDC Coords
    // std::vector<float> vertices = {
    //     -1.0f, -1.0f,
    //     1.0f, -1.0f,
    //     -1.0f, 1.0f,
    //     1.0f, 1.0f
    // };

    // glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GL_FLOAT), vertices.data(), GL_STATIC_DRAW);

    // glEnableVertexAttribArray(1);
    // glBindBuffer(GL_ARRAY_BUFFER, screenObject.uvBuffer);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

    // std::vector<float> uvs = {
    //     0.0f, 0.0f,
    //     1.0f, 0.0f,
    //     0.0f, 1.0f,
    //     1.0f, 1.0f
    // };

    // glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(GL_FLOAT), uvs.data(), GL_STATIC_DRAW);
}

void Renderer::addModel(std::shared_ptr<Model> model) {
    model->setProjectionAndViewMatrices(camera->getProjectionMatrix(), camera->getViewMatrix());
    model->setLights(lights);
    models.push_back(model);
}

void Renderer::addLight(std::shared_ptr<Light> light) {
    lights.push_back(light);

    for (auto& model : models) {
        model->setLights(lights);
    }
}

void Renderer::updateCameraRotation(glm::vec3 r) {
    camera->addRotation(r);
}

void Renderer::render() {
    glViewport(0, 0, width, height);

    // Bind the scene buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (camera->isDirty()) {
        for (auto& model : models) {
            model->setProjectionAndViewMatrices(camera->getProjectionMatrix(), camera->getViewMatrix());
        }
        camera->setDirty(false);
    }
    // TODO(mfirmin): Check if lights are dirty
    for (auto& model : models) {
        model->setLights(lights);
    }

    for (auto& model : models) {
        model->applyModelMatrix();
        model->draw(MaterialType::standard);
    }

    glUseProgram(0);

    // // Bind the screen framebuffer
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // glClearColor(0.0, 0.0, 0.0, 1.0);
    // // Clear it
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // // render the screen object to it (using the scene render target)
    // glBindVertexArray(screenObject.vertexArray);
    // glUseProgram(compositingPass.program);

    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, sceneTarget->getTexture());

    // glActiveTexture(GL_TEXTURE1);
    // glBindTexture(GL_TEXTURE_2D, bloomEffect.getBlurTexture());

    // glUniform1i(glGetUniformLocation(compositingPass.program, "scene"), 0);
    // glUniform1i(glGetUniformLocation(compositingPass.program, "bloomBlur"), 1);

    // glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // glUseProgram(0);

    // Swap
    SDL_GL_SwapWindow(window);
}
