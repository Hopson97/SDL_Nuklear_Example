#pragma once

#include <cglm/cglm.h>
#include <glad/glad.h>

#define ARR_LENGTH(a) (sizeof(a) / sizeof(a[0]))
#define MAKE_VERTEX_ARRAY(vertices, indices)                                             \
    makeVertexArray(vertices, indices, ARR_LENGTH(vertices), ARR_LENGTH(indices));

struct Vertex {
    vec3 position;
    vec2 texture;
};

struct Renderable {
    GLuint vao;
    GLuint indices;
};

struct VertexArray {
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint numIndices;
};

struct VertexArray makeVertexArray(const struct Vertex* vertices, const GLuint* indices,
                                   GLsizei numVerticies, GLsizei numIndices);
void destroyVertexArray(struct VertexArray* vertexArray);
