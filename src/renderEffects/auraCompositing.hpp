#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include <memory>
#include <vector>

class Light;

class AuraCompositingEffect {
    public:
        AuraCompositingEffect(int width, int height);

        AuraCompositingEffect(AuraCompositingEffect&& other) = default;
        AuraCompositingEffect& operator=(AuraCompositingEffect&& other) = default;

        AuraCompositingEffect(const AuraCompositingEffect& other) = delete;
        AuraCompositingEffect& operator=(const AuraCompositingEffect& other) = delete;

        ~AuraCompositingEffect();

        void initialize();

        GLuint getProgram() {
            return program;
        }

        void render(GLuint sceneTexture, GLuint auraTexture);
    private:
        int width;
        int height;

        GLuint program = 0;
        GLuint debugProgram = 0;
        GLuint vao = 0;
        GLuint vertexBuffer = 0;
        GLuint uvBuffer = 0;

        void createProgram();
        void createDebugProgram();
        void createArrayObjects();
};
