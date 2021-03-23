#include "renderTarget.hpp"

#include <iostream>

RenderTarget::RenderTarget(int w, int h) :
    width(w), height(h)
{
    // initialize the framebuffer for the render target
    glGenFramebuffers(1, &msFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, msFBO);

    // Initialize the color buffer for the multisample fbo
    glGenRenderbuffers(1, &colorBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBuffer);
    // use a floating point renderbuffer for the color
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGBA16F, width, height);
    // attach the color buffer to the frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBuffer);

    // initialize the depth buffer for the multisample fbo
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_DEPTH_COMPONENT, width, height);
    // attach the depth buffer to the frame buffer
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    GLenum drawbuffers[1] = { GL_COLOR_ATTACHMENT0 };

    glDrawBuffers(1, static_cast<GLenum*>(drawbuffers));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating RenderTarget: Error creating multisample framebuffer\n";
        return;
    }


    // INITIALIZE THE OUTPUT buffer (the non-multisampled one)
    // initialize the framebuffer for the render target
    glGenFramebuffers(1, &outFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, outFBO);

    // initialize the texture to bind to the buffer
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // floating point texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // attach the texture to the framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    glDrawBuffers(1, static_cast<GLenum*>(drawbuffers));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Error creating RenderTarget: Error creating output framebuffer\n";
        return;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget() {
    // TODO: Free buffers
}
