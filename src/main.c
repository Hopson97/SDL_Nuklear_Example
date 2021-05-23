#include "Camera.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/GLDebug.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexArray.h"
#include "Maths.h"
#include "Utility.h"
#include <SDL2/SDL.h>
#include <cglm/cam.h>
#include <cglm/struct.h>
#include <cute_headers/cute_sound.h>
#include <glad/glad.h>
#include <nuklear/nuklear_def.h>
#include <nuklear/nuklear_sdl_gl3.h>
#include <stdbool.h>

#define MAX_VERTEX_MEMORY 0x80000
#define MAX_ELEMENT_MEMORY 0x20000

int main(void)
{
    SDL_Window* window = initWindow();
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glViewport(0, 0, WIDTH, HEIGHT);

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

    cs_context_t* audioContext = cs_make_context(NULL, loaded.sample_rate, 1024 * 2, 0, NULL);

    cs_playing_sound_t jump = cs_make_playing_sound(&loaded);
    cs_spawn_mix_thread(audioContext);

    cs_set_volume(&jump, 0.5, 0.5);
    cs_insert_sound(audioContext, &jump);

    //=======================================
    //          OPENGL OBJECT SETUP
    //=======================================
    // clang-format off
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
    struct VertexArray quad = CREATE_VERTEX_ARRAY(vertices, indices);
    struct VertexArray screen = createEmptyVertexArray();

    GLuint shader = loadShaders("MinVertex.glsl", "MinFragment.glsl");
    GLuint frameBufferShader = loadShaders("FramebufferVertex.glsl", "FramebufferFragment.glsl");
    GLuint texture = loadTexture("opengl_logo.png");

    struct Framebuffer framebuffer = createFramebuffer(WIDTH, HEIGHT);

    //=======================================
    //          WOW LETS MAKE IT 3D
    //=======================================

    struct Camera camera = createCamera();

    float aspect = (float)WIDTH / (float)HEIGHT;
    Matrix4 projectionMatrix;
    glm_perspective(glm_rad(90.0f), aspect, 0.1f, 100.0f, projectionMatrix);

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

    struct VertexArray terrain = createTerrainVertexArray();
    //=======================================
    //          MAIN LOOP
    //=======================================
    const Uint8* keyboard = NULL;
    bool running = true;
    int start = SDL_GetTicks();
    int frames = 0;
    int fps = 0;
    while (running) {
        frames++;
        int timeNow = SDL_GetTicks();
        if (timeNow - start >= 1000) {
            fps = frames;
            frames = 0;
            start = timeNow;
        }
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

                case SDL_MOUSEMOTION:
                    cameraMouseInput(&camera, event.motion.xrel, event.motion.yrel);
                    break;

                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        keyboard = SDL_GetKeyboardState(NULL);

        //=======================================
        //              INPUT
        //=======================================
        cameraKeyboardInput(&camera, keyboard);
        // Update
        Matrix4 projectionViewMatrix = MATRIX4_IDENTITY;
        cameraUpdate(&camera, projectionViewMatrix);

        // GUI
        if (nk_begin(ctx, "Debug Window", nk_rect(0, 0, 400, 200), 0)) {
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Position: (%f %f %f)", camera.position[0], camera.position[1],
                      camera.position[2]);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Rotation: (%f %f %f)", camera.rotation[0], camera.rotation[1],
                      camera.rotation[2]);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "Player Front: (%f %f %f)", camera.front[0], camera.front[1], camera.front[2]);
            nk_layout_row_dynamic(ctx, 25, 1);
            nk_labelf(ctx, NK_STATIC, "FPS: %d", fps);
        }
        nk_end(ctx);

        //=======================================
        //          Render
        //=======================================
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader);

        loadMatrix4ToShader(shader, "projectionViewMatrix", projectionViewMatrix);

        // Bind stuff then render
        glBindVertexArray(quad.vao);
        glBindTextureUnit(0, texture);
        for (int i = 0; i < count; i++) {
            Matrix4 modelMatrix = MATRIX4_IDENTITY;
            createModelMatrix(modelLocations[i], modelRotations[i], modelMatrix);
            loadMatrix4ToShader(shader, "modelMatrix", modelMatrix);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(terrain.vao);

        Matrix4 modelMatrix = MATRIX4_IDENTITY;
        vec3 loc = VECTOR3_ZERO;
        createModelMatrix(loc, loc, modelMatrix);
        loadMatrix4ToShader(shader, "modelMatrix", modelMatrix);
        glDrawElements(GL_TRIANGLES, terrain.numIndices, GL_UNSIGNED_INT, 0);

        // Final render pass
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(frameBufferShader);
        glBindTextureUnit(0, framebuffer.colourAttachment);

        glBindVertexArray(screen.vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        SDL_GL_SwapWindow(window);
    }

    //=======================================
    //          CLEAN UP
    //=======================================
    destroyVertexArray(&quad);
    destroyVertexArray(&terrain);
    destroyVertexArray(&screen);

    cs_free_sound(&loaded);
    cs_shutdown_context(audioContext);

    // OpenGL
    glDeleteProgram(shader);
    glDeleteProgram(frameBufferShader);

    // Nuklear
    nk_sdl_shutdown();

    // SDL
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
