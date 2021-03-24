#pragma once

#include <GL/glew.h>


class RenderTarget {
    public:
        RenderTarget(int width, int height, bool withStencil = false);

        RenderTarget(RenderTarget&& other) = default;
        RenderTarget& operator=(RenderTarget&& other) = default;

        RenderTarget(const RenderTarget& other) = delete;
        RenderTarget& operator=(const RenderTarget& other) = delete;

        ~RenderTarget();

        GLuint getFramebuffer() {
            return fbo;
        }

        GLuint getColorTexture() {
            return colorTexture;
        }

        GLuint getDepthBuffer() {
            return depthBuffer;
        }
    private:
        int width;
        int height;

        GLuint fbo = 0;
        GLuint colorTexture = 0;
        GLuint depthBuffer = 0;
};
