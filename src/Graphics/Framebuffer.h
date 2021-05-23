#pragma once

#include <glad/glad.h>

struct Framebuffer {
    GLuint fbo;
    GLuint rbo;

    GLuint colourAttachment;

    GLuint width;
    GLuint height;
};

struct Framebuffer createFramebuffer(GLuint width, GLuint height);
void destroyFramebuffer(struct Framebuffer* framebuffer);
