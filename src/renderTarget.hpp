#pragma once

#include <GL/glew.h>


class RenderTarget {
    public:
        RenderTarget(int width, int height);

        RenderTarget(RenderTarget&& other) = default;
        RenderTarget& operator=(RenderTarget&& other) = default;

        RenderTarget(const RenderTarget& other) = delete;
        RenderTarget& operator=(const RenderTarget& other) = delete;

        ~RenderTarget();

        GLuint getMultiSampleFramebuffer() {
            return msFBO;
        }

        GLuint getOutputFramebuffer() {
            return outFBO;
        }

        GLuint getTexture() {
            return texture;
        }
    private:
        int width;
        int height;

        GLuint msFBO = 0;
        GLuint colorBuffer = 0;
        GLuint depthBuffer = 0;

        GLuint outFBO = 0;
        GLuint texture = 0;
};
