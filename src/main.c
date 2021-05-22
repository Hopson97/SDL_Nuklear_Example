#include "Graphics/GLDebug.h"
#include "Graphics/Shader.h"
#include "Maths.h"
#include <SDL2/SDL.h>
#include <cglm/cam.h>
#include <cglm/struct.h>
#include <cute_headers/cute_sound.h>
#include <glad/glad.h>
#include <nuklear/nuklear_def.h>
#include <nuklear/nuklear_sdl_gl3.h>
#include <stb/stb_image.h>
#include <stb/stb_perlin.h>
#include <stdbool.h>

#define MAX_VERTEX_MEMORY 0x80000
#define MAX_ELEMENT_MEMORY 0x20000

#define WIDTH 1600
#define HEIGHT 900

int main(void)
{
    //=======================================
    //          SDL SET UP SECTION
    //=======================================
    SDL_Init(SDL_INIT_EVERYTHING);

    // Set up SDL to work with OpenGL (https://wiki.libsdl.org/SDL_GLattr)
    // Enable hardware acceleration
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Set rendering parmeters
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Create the window and OpenGL context

    SDL_Window* window = SDL_CreateWindow(
        "OpenGL Nuklear Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH,
        HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GLContext context = SDL_GL_CreateContext(window);
    // Init OpenGL functions
    if (!gladLoadGL()) {
        printf("Error: Could not load OpenGL.");
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }
    initGLDebug();

    //=======================================
    //          NUKLEAR SET UP SECTION
    //=======================================
    struct nk_context* ctx;
    // set_style(ctx, THEME_WHITE);
    ctx = nk_sdl_init(window);
    {
        struct nk_font_atlas* atlas;
        nk_sdl_font_stash_begin(&atlas);
        /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas,
         * "../../../extra_font/DroidSans.ttf", 14, 0);*/
        nk_sdl_font_stash_end();
    }
    nk_set_style(ctx, THEME_DARK);

    //=======================================
    //          CUTE SOUNDS
    //=======================================
    cs_loaded_sound_t loaded = cs_load_wav("Data/file_example_WAV_1MG.wav");
    fflush(stdout);

    cs_context_t* audioContext =
        cs_make_context(NULL, loaded.sample_rate, 1024 * 2, 0, NULL);

    cs_playing_sound_t jump = cs_make_playing_sound(&loaded);
    cs_spawn_mix_thread(audioContext);

    cs_set_volume(&jump, 0.5, 0.5);
    cs_insert_sound(audioContext, &jump);

    //=======================================
    //          OPENGL OBJECT SETUP
    //=======================================
    // clang-format off

    struct Vertex
    {
        vec3 position;
        vec2 texture;
    };

    struct Vertex vertices[4] = {

        {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f}},
        {{0.5f, -0.5f, 0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f}},
    };

    const GLuint indices[]  = {
        0, 1, 2, 2, 3, 0
    };
    // clang-format on

    //
    //  Create vertex data
    //
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;

    // glGenBuffers, glGenVertexArrays
    glCreateVertexArrays(1, &vao);
    glCreateBuffers(1, &vbo);
    glCreateBuffers(1, &ebo);

    // glBufferData
    glNamedBufferStorage(vbo, sizeof(struct Vertex) * 4, vertices,
                         GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(ebo, sizeof(GLuint) * 6, indices, GL_DYNAMIC_STORAGE_BIT);

    // Attach the vertex array to the vertex buffer and element buffer
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(struct Vertex));
    glVertexArrayElementBuffer(vao, ebo);

    // glEnableVertexAttribArray
    glEnableVertexArrayAttrib(vao, 0);
    glEnableVertexArrayAttrib(vao, 1);

    // glVertexAttribPointer
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE,
                              offsetof(struct Vertex, position));
    glVertexArrayAttribFormat(vao, 1, 2, GL_FLOAT, GL_FALSE,
                              offsetof(struct Vertex, texture));
    glVertexArrayAttribBinding(vao, 0, 0);
    glVertexArrayAttribBinding(vao, 1, 0);

    // Load shaders
    GLuint shader =
        loadShaders("Data/Shaders/MinVertex.glsl", "Data/Shaders/MinFragment.glsl");

    //
    // Load up a texture
    //

    int width;
    int height;
    int channels;
    unsigned char* imageData = stbi_load("Data/Textures/opengl_logo.png", &width, &height,
                                         &channels, STBI_rgb_alpha);

    GLuint texture;
    glCreateTextures(GL_TEXTURE_2D, 1, &texture);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureStorage2D(texture, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                        imageData);
    stbi_image_free(imageData);

    //=======================================
    //          OPENGL MISC SETUP
    //=======================================
    // glDisable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glViewport(0, 0, WIDTH, HEIGHT);

    //=======================================
    //          WOW LETS MAKE IT 3D
    //=======================================
    float aspect = (float)WIDTH / (float)HEIGHT;
    Matrix4 projectionMatrix;
    glm_perspective(glm_rad(90.0f), aspect, 0.1f, 100.0f, projectionMatrix);

    // Camera stuff
    Vector3 playerPosition = VECTOR3_ZERO;
    Vector3 playerRotation = {0, -90.0f, 0.0};
    Vector3 up = {0, 1, 0};
    Vector3 front = VECTOR3_ZERO;

    // Scene objects
    int count = 100;
    Vector3 modelLocations[count];
    Vector3 modelRotations[count];

    for (int i = 0; i < count; i++) {
        modelLocations[i][0] = rand() % 75;
        modelLocations[i][1] = rand() % 75;
        modelLocations[i][2] = rand() % 75;

        modelRotations[i][0] = rand() % 360;
        modelRotations[i][1] = rand() % 360;
        modelRotations[i][2] = rand() % 360;
    }


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

    //
    //  Create vertex data
    //
    GLuint terrainvao = 0;
    GLuint terrainvbo = 0;
    GLuint terrainebo = 0;

    // glGenBuffers, glGenVertexArrays
    glCreateVertexArrays(1, &terrainvao);
    glCreateBuffers(1, &terrainvbo);
    glCreateBuffers(1, &terrainebo);

    // glBufferData
    glNamedBufferStorage(terrainvbo, sizeof(struct Vertex) * VERTS * VERTS, terrainVerts, GL_DYNAMIC_STORAGE_BIT);
    glNamedBufferStorage(terrainebo, sizeof(GLuint) * indicesCount, terrainIndices, GL_DYNAMIC_STORAGE_BIT);

    // Attach the vertex array to the vertex buffer and element buffer
    glVertexArrayVertexBuffer(terrainvao, 0, terrainvbo, 0, sizeof(struct Vertex));
    glVertexArrayElementBuffer(terrainvao, terrainebo);

    // glEnableVertexAttribArray
    glEnableVertexArrayAttrib(terrainvao, 0);
    glEnableVertexArrayAttrib(terrainvao, 1);

    // glVertexAttribPointer
    glVertexArrayAttribFormat(terrainvao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, position));
    glVertexArrayAttribFormat(terrainvao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(struct Vertex, texture));
    glVertexArrayAttribBinding(terrainvao, 0, 0);
    glVertexArrayAttribBinding(terrainvao, 1, 0);

    //=======================================
    //          MAIN LOOP
    //=======================================
    const Uint8* keyboard = NULL;
    bool running = true;
    while (running) {
        SDL_Delay(16); //"hack" for 60fps
        nk_input_begin(ctx);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            nk_sdl_handle_event(&event);
            switch (event.type) {
                case SDL_KEYUP:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        running = false;
                    }
                    break;

                case SDL_MOUSEMOTION: {
                    int mouseXDiff = event.motion.xrel;
                    int mouseYDiff = event.motion.yrel;
                    playerRotation[0] -= mouseYDiff / 4.0f;
                    playerRotation[1] += mouseXDiff / 4.0f;

                    playerRotation[0] = glm_clamp(playerRotation[0], -89.9f, 89.9f);
                } break;

                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        //=======================================
        //              INPUT
        //=======================================

        // Keyboard input
        // https://wiki.libsdl.org/SDL_Scancode
        keyboard = SDL_GetKeyboardState(NULL);
        if (keyboard[SDL_SCANCODE_W]) {
            moveVectorForwards(playerPosition, playerRotation, 1);
        }
        if (keyboard[SDL_SCANCODE_A]) {
            moveVectorLeft(playerPosition, playerRotation, 1);
        }
        if (keyboard[SDL_SCANCODE_S]) {
            moveVectorBackwards(playerPosition, playerRotation, 1);
        }
        if (keyboard[SDL_SCANCODE_D]) {
            moveVectorRight(playerPosition, playerRotation, 1);
        }

        // Update

        // GUI
        if (nk_begin(ctx, "Debug Window", nk_rect(0, 0, 400, 200), 0)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Position: (%f %f %f)", playerPosition[0],
                      playerPosition[1], playerPosition[2]);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Rotation: (%f %f %f)", playerRotation[0],
                      playerRotation[1], playerRotation[2]);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Front: (%f %f %f)", front[0], front[1],
                      front[2]);
        }
        nk_end(ctx);

        //=======================================
        //          Render
        //=======================================
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // View Matrix
        Vector3 center;

        front[0] = cos(glm_rad(playerRotation[1])) * cos(glm_rad(playerRotation[0]));
        front[1] = sin(glm_rad(playerRotation[0]));
        front[2] = sin(glm_rad(playerRotation[1])) * cos(glm_rad(playerRotation[0]));
        glm_normalize(front);
        glm_vec3_add(playerPosition, front, center);

        Matrix4 viewMatrix = MATRIX4_IDENTITY;
        glm_lookat(playerPosition, center, up, viewMatrix);

        // Calculate projection view matrix and then upload
        Matrix4 projectionViewMatrix = MATRIX4_IDENTITY;
        glm_mat4_mul(projectionMatrix, viewMatrix, projectionViewMatrix);

        glUseProgram(shader);
        loadMatrix4ToShader(shader, "projectionViewMatrix", projectionViewMatrix);

        // Bind stuff then render
        glBindVertexArray(vao);
        glBindTextureUnit(0, texture);
        for (int i = 0; i < count; i++) {
            Matrix4 modelMatrix = MATRIX4_IDENTITY;
            createModelMatrix(modelLocations[i], modelRotations[i], modelMatrix);
            loadMatrix4ToShader(shader, "modelMatrix", modelMatrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(terrainvao);

        Matrix4 modelMatrix = MATRIX4_IDENTITY;
        vec3 loc = VECTOR3_ZERO;
        createModelMatrix(loc, loc, modelMatrix);
        loadMatrix4ToShader(shader, "modelMatrix", modelMatrix);
        glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(window);
    }

    //=======================================
    //          CLEAN UP
    //=======================================

    cs_free_sound(&loaded);
    cs_shutdown_context(audioContext);

    // OpenGL
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(shader);

    // Nuklear
    nk_sdl_shutdown();

    // SDL
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
