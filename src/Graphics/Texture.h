#pragma once

#include <glad/glad.h>

GLuint loadTexture(const char* file);
GLuint createFramebufferTexture(GLint width, GLint height);
