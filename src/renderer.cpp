#include "renderer.hpp"

#include "camera.hpp"
#include "gl/shaderUtils.hpp"
#include "light/light.hpp"
#include "material/material.hpp"
#include "model.hpp"
#include "renderTarget.hpp"
#include "renderEffects/auraCompositing.hpp"

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

    auraTarget = std::make_unique<RenderTarget>(width, height, true);
    sceneTarget = std::make_unique<RenderTarget>(width, height, false);
    auraEffect = std::make_unique<AuraCompositingEffect>(width, height);
    auraEffect->initialize();

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
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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
    glEnable(GL_STENCIL_TEST);
    // Enable writing to depth buffer
    glDepthMask(GL_TRUE);

    // necessary for cubemaps to work correctly
    // glDepthFunc(GL_LEQUAL);
    // Enable MultiSampling
    // glEnable(GL_MULTISAMPLE);
    // Enable face culling
    glEnable(GL_CULL_FACE);

    int bits;
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &bits);
    std::cout << "Stencil Size: " << bits << "\n";

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
    // --- UPDATE UNIFORMS ---

    if (camera->isDirty()) {
        for (auto& model : models) {
            model->setProjectionAndViewMatrices(camera->getProjectionMatrix(), camera->getViewMatrix());
        }
        camera->setDirty(false);
    }
    // TODO(mfirmin): Check if lights are dirty
    for (auto& model : models) {
        model->setLights(lights);
        model->applyModelMatrix();
    }

    // --- RENDER STENCIL/AURA ---
    {
        // Bind the aura/stencil buffer
        glBindFramebuffer(GL_FRAMEBUFFER, auraTarget->getFramebuffer());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

        // render 1 to each fragment
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        // Clear the stencil buffer

        // always draw fragments regardless of stencil value
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDisable(GL_STENCIL_TEST);
        glStencilMask(0x00);
        // record depth into framebuffer
        // glDepthMask(GL_FALSE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        // draw all aura materials into DEPTH BUFFER ONLY
        for (auto& model : models) {
            model->draw(MaterialType::aura);
        }


        // then draw the stencil only where it passes the depth test
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);

        for (auto& model : models) {
            model->draw(MaterialType::stencil);
        }

        glClear(GL_DEPTH_BUFFER_BIT);

        // then render the aura's color only where the stencil is valid
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        // only render the aura where the stencil is
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        // do not write to stencil
        glStencilMask(0x00);

        for (auto& model : models) {
            model->draw(MaterialType::aura);
        }

        glDisable(GL_STENCIL_TEST);
    }

    // --- RENDER SCENE ---
    {
        glBindFramebuffer(GL_FRAMEBUFFER, sceneTarget->getFramebuffer());
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (auto& model : models) {
            model->draw(MaterialType::standard);
        }
    }

    // --- Composite and render to screen ---
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auraEffect->render(sceneTarget->getColorTexture(), auraTarget->getColorTexture());
    }

    glUseProgram(0);

    // Swap
    SDL_GL_SwapWindow(window);
}
