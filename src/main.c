#include "Camera.h"
#include "GUI.h"
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
#include <stdbool.h>

int main(void)
{
    SDL_Window* window = initWindow();
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glViewport(0, 0, WIDTH, HEIGHT);

    guiInit(window);

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
    bool running = true;
    while (running) {
        guiBeginFrame();
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            guiProcessEvent(&event);
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

        //  Input
        cameraKeyboardInput(&camera);

        // Update
        Matrix4 projectionViewMatrix = MATRIX4_IDENTITY;
        cameraUpdate(&camera, projectionViewMatrix);

        guiDebugScreen(&camera);

        // Render
        glEnable(GL_DEPTH_TEST);

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

        guiEndFrame();
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
    guiShutdown();
    // SDL
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
