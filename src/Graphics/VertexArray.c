#include "VertexArray.h"
#include <stb/stb_perlin.h>

static void bufferVertexData(struct VertexArray* v, const struct Vertex* vertices, GLsizei numVerticies)
{
    glCreateBuffers(1, &v->vbo);

    // glBufferData
    glNamedBufferStorage(v->vbo, sizeof(struct Vertex) * numVerticies, vertices, GL_DYNAMIC_STORAGE_BIT);

    // Attach the vertex array to the vertex buffer and element buffer
    glVertexArrayVertexBuffer(v->vao, 0, v->vbo, 0, sizeof(struct Vertex));

    // glEnableVertexAttribArray
    glEnableVertexArrayAttrib(v->vao, 0);
    glEnableVertexArrayAttrib(v->vao, 1);

    // glVertexAttribPointer
    glVertexArrayAttribFormat(v->vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, position));
    glVertexArrayAttribFormat(v->vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, texture));
    glVertexArrayAttribBinding(v->vao, 0, 0);
    glVertexArrayAttribBinding(v->vao, 1, 0);
}

static void bufferIndicesData(struct VertexArray* v, const GLuint* indices, GLsizei numIndices)
{
    glCreateBuffers(1, &v->ibo);
    glNamedBufferStorage(v->ibo, sizeof(GLuint) * numIndices, indices, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayElementBuffer(v->vao, v->ibo);
    v->numIndices = numIndices;
}

struct VertexArray makeVertexArray(const struct Vertex* vertices, const GLuint* indices, GLsizei numVerticies,
                                   GLsizei numIndices)
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

struct VertexArray genTerrainVertexArray()
{
    // Generate some spicy terrain
#define VERTS 128
#define SIZE 50
    struct Vertex terrainVerts[VERTS * VERTS];
    int ptr = 0;

    for (int z = 0; z < VERTS; z++) {
        for (int x = 0; x < VERTS; x++) {
            float fx = (float)x;
            float fy = (float)z;

            float vx = fx / (VERTS - 1) * SIZE;
            float vz = fy / (VERTS - 1) * SIZE;

            // Begin iterating through the octaves

            int octaves = 5;
            float smooth = 200;
            float roughness = 0.58f;

            float value = 0;
            float accumulatedAmps = 0;
            for (int i = 0; i < octaves; i++) {
                float frequency = powf(2.0f, i);
                float amplitude = powf(roughness, i);

                float nx = x * frequency / smooth;
                float nz = z * frequency / smooth;

                float noise = stb_perlin_noise3_seed(nx, 4221, nz, 0, 0, 0, 21421);
                noise = (noise + 1.0f) / 2.0f;
                value += noise * amplitude;
                accumulatedAmps += amplitude;
            }
            float height = (value / accumulatedAmps) * 100;

            struct Vertex vertex;
            vertex.position[0] = vx;
            vertex.position[1] = height - 100;
            vertex.position[2] = vz;

            float u = fx / VERTS - 1;
            float v = fy / VERTS - 1;
            vertex.texture[0] = u;
            vertex.texture[1] = v;

            terrainVerts[ptr++] = vertex;
        }
    }

    int indicesCount = 0;
    ptr = 0;
    GLuint terrainIndices[VERTS * VERTS * 10];
    for (int y = 0; y < (VERTS - 1); y += 1) {
        for (int x = 0; x < (VERTS - 1); x += 1) {
            int topLeft = (y * VERTS) + x;
            int topRight = topLeft + 1;
            int bottomLeft = ((y + 1) * VERTS) + x;
            int bottomRight = bottomLeft + 1;

            terrainIndices[ptr++] = topLeft;
            terrainIndices[ptr++] = bottomLeft;
            terrainIndices[ptr++] = topRight;
            terrainIndices[ptr++] = bottomLeft;
            terrainIndices[ptr++] = bottomRight;
            terrainIndices[ptr++] = topRight;

            indicesCount += 6;
        }
    }

    return MAKE_VERTEX_ARRAY(terrainVerts, terrainIndices);
}
