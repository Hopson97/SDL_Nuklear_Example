#pragma once

#include <glad/glad.h>
#include <stdbool.h>
#include <cglm/include/cglm/cglm.h>
#include <cglm/include/cglm/mat4.h>

struct UniformLocation;

typedef struct Shader {
    GLuint program;
} Shader; 


GLuint loadShaders(const char* vertexFilename, const char* fragmentFileName);

void shaderLoadUniformMatrix4(Shader* shader, const char* name, const mat4 matrix);
