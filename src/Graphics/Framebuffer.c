#include "Framebuffer.h"

#include "Texture.h"
#include "../Utility.h"
#include <stdio.h>

struct Framebuffer createFramebuffer(GLuint width, GLuint height)
{
    // Create framebuffer
    struct Framebuffer framebuffer = {0};
    glCreateFramebuffers(1, &framebuffer.fbo);
    framebuffer.width = width;
    framebuffer.height = height;

    // Create and attach colour texture attachment
    framebuffer.colourAttachment = createFramebufferTexture(width, height);
    glNamedFramebufferTexture(framebuffer.fbo, GL_COLOR_ATTACHMENT0, framebuffer.colourAttachment, 0);

    // Create a render buffer and attach it
    glCreateRenderbuffers(1, &framebuffer.rbo);
    glNamedRenderbufferStorage(framebuffer.rbo, GL_DEPTH24_STENCIL8, width, height);
    glNamedFramebufferRenderbuffer(framebuffer.fbo, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, framebuffer.rbo);

    if (glCheckNamedFramebufferStatus(framebuffer.fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        setTextColourRGB(255, 0, 0);
        fprintf(stderr, "Failed to create framebuffer.");
        setTextColourRGB(255, 255, 255);
        fflush(stderr);
    }
    return framebuffer;
}

void destroyFramebuffer(struct Framebuffer* framebuffer)
{
    glDeleteRenderbuffers(1, &framebuffer->rbo);
    glDeleteFramebuffers(1, &framebuffer->fbo);
    glDeleteTextures(1, &framebuffer->colourAttachment);
}
