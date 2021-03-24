#include "renderTarget.hpp"

#include <assert.h>
#include <array>
#include <iostream>

RenderTarget::RenderTarget(int w, int h, bool withStencil) :
    width(w), height(h)
{
    const unsigned int numTextures = 1;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    std::array<GLuint*, numTextures> textures = {
        &colorTexture,
    };

    std::array<GLint, numTextures> internalformats = { GL_RGBA };
    std::array<GLint, numTextures> formats = { GL_RGBA, };
    std::array<GLenum, numTextures> types = { GL_UNSIGNED_BYTE };
    std::array<GLenum, numTextures> attachments = { GL_COLOR_ATTACHMENT0 };

    for (unsigned int i = 0; i < numTextures; i++) {
        GLuint* texture = textures.at(i);
        glGenTextures(1, texture);

        glBindTexture(GL_TEXTURE_2D, *texture);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformats.at(i), width, height, 0, formats.at(i), types.at(i), nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, attachments.at(i), GL_TEXTURE_2D, *texture, 0);
    }

    GLuint depthFormat = withStencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT;
    GLuint depthAttachment = withStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;

    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, depthAttachment, GL_RENDERBUFFER, depthBuffer);

    glDrawBuffers(numTextures, attachments.data());

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating DeferredPBREffect: Error creating framebuffer\n";
        assert(false);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
    // TODO: Free buffers
}
