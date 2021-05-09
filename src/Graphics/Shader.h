#pragma once

#include <cglm/include/cglm/cglm.h>
#include <cglm/include/cglm/mat4.h>
#include <glad/glad.h>
#include <stdbool.h>

GLuint loadShaders(const char* vertexFilename, const char* fragmentFileName);

void loadMatrix4ToShader(GLuint shader, const char* name, const mat4 matrix);
