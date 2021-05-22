#include "VertexArray.h"

static void bufferVertexData(struct VertexArray* v, const struct Vertex* vertices,
                             GLsizei numVerticies)
{
    glCreateBuffers(1, &v->vbo);

    // glBufferData
    glNamedBufferStorage(v->vbo, sizeof(struct Vertex) * numVerticies, vertices,
                         GL_DYNAMIC_STORAGE_BIT);

    // Attach the vertex array to the vertex buffer and element buffer
    glVertexArrayVertexBuffer(v->vao, 0, v->vbo, 0, sizeof(struct Vertex));

    // glEnableVertexAttribArray
    glEnableVertexArrayAttrib(v->vao, 0);
    glEnableVertexArrayAttrib(v->vao, 1);

    // glVertexAttribPointer
    glVertexArrayAttribFormat(v->vao, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(struct Vertex, position));
    glVertexArrayAttribFormat(v->vao, 1, 2, GL_FLOAT, GL_FALSE,
                              offsetof(struct Vertex, texture));
    glVertexArrayAttribBinding(v->vao, 0, 0);
    glVertexArrayAttribBinding(v->vao, 1, 0);
}

static void bufferIndicesData(struct VertexArray* v, const GLuint* indices,
                              GLsizei numIndices)
{
    glCreateBuffers(1, &v->ibo);
    glNamedBufferStorage(v->ibo, sizeof(GLuint) * numIndices, indices,
                         GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(v->vao, v->ibo);
    v->numIndices = numIndices;
}

struct VertexArray makeVertexArray(const struct Vertex* vertices, const GLuint* indices,
                                   GLsizei numVerticies, GLsizei numIndices)
{
    struct VertexArray v = {0};
    glCreateVertexArrays(1, &v.vao);
    bufferVertexData(&v, vertices, numVerticies);
    bufferIndicesData(&v, indices, numIndices);
    return v;
}

void destroyVertexArray(struct VertexArray* v)
{
    glDeleteBuffers(1, &v->ibo);
    glDeleteBuffers(1, &v->vbo);
    glDeleteVertexArrays(1, &v->vao);
    v->numIndices = 0;
}
