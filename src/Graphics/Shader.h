#pragma once

#include "../Maths.h"
#include <glad/glad.h>
#include <stdbool.h>

GLuint loadShaders(const char* vertexFilename, const char* fragmentFileName);

void loadFloatToShader(GLuint shader, float value);
void loadMatrix4ToShader(GLuint shader, const char* name, const Matrix4 matrix);
