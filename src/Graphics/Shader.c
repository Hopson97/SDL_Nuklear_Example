#include "Shader.h"

#include "../Utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char* getShaderString(GLenum shaderType)
{
    if (shaderType == GL_VERTEX_SHADER) {
        return "Vertex Shader";
    }
    else if (shaderType == GL_FRAGMENT_SHADER) {
        return "Fragment Shader";
    }
    else {
        return "Unknown shader";
    }
}

static bool compileShader(GLuint* shaderOut, const char* fileName, GLenum shaderType)
{
    char* source = getFileContent(fileName);
    if (!source) {
        fprintf(stderr, "Failed to load %s file.\n", getShaderString(shaderType));
        return false;
    }
    GLuint shader = glCreateShader(shaderType);

    int length = strlen(source);
    glShaderSource(shader, 1, (const GLchar* const*)&source, &length);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char buff[1024];
        glGetShaderInfoLog(shader, 1024, NULL, buff);
        fprintf(stderr, "%s compilation failed: %s\n", getShaderString(shaderType), buff);
        free(source);
        return false;
    }
    *shaderOut = shader;
    free(source);
    return true;
}

GLuint loadShaders(const char* vertexFilename, const char* fragmentFileName)
{
    char vertfullFileName[128] = "Data/Shaders/";
    char fragfullFileName[128] = "Data/Shaders/";
    strcat(vertfullFileName, vertexFilename);
    strcat(fragfullFileName, fragmentFileName);

    GLuint vertexShader;
    GLuint fragmentShader;
    if (!compileShader(&vertexShader, vertfullFileName, GL_VERTEX_SHADER)) {
        exit(1);
    }

    if (!compileShader(&fragmentShader, fragfullFileName, GL_FRAGMENT_SHADER)) {
        exit(1);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glBindAttribLocation(program, 0, "inPosition");

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char buff[1024];
        glGetShaderInfoLog(program, 1024, NULL, buff);
        fprintf(stderr, "Failed to link shader programs: %s\n", buff);
        exit(1);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}

void loadMatrix4ToShader(GLuint shader, const char* name, const Matrix4 matrix)
{
    GLuint location = glGetUniformLocation(shader, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, (float*)matrix);
}
