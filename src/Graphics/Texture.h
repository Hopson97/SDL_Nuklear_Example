#pragma once

#include <glad/glad.h>

typedef struct Texture2D {
    GLuint id;
} Texture2D;

Texture2D load2DTexture(const char* fileName);
