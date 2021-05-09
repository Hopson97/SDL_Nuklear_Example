#pragma once

#include <glad/glad.h>
#include <stdbool.h>

#include <cglm/struct.h>

GLuint loadShaders(const char* vertexFilename, const char* fragmentFileName);

void loadMatrix4ToShader(GLuint shader, const char* name, const mat4s matrix);
