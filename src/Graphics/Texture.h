#pragma once

#include <glad/glad.h>

struct Texture2D {
    GLuint id;
};

struct Texture2D load2DTexture(const char* fileName);
