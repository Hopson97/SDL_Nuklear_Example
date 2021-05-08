#define GL_GLEXT_PROTOTYPES

#include "Graphics/GLDebug.h"
#include "Graphics/Shader.h"
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <nuklear/nuklear_def.h>
#include <nuklear/nuklear_sdl_gl3.h>
#include <stdbool.h>

#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024

#define WIDTH 1600
#define HEIGHT 900

int main(void)
{
    //=======================================
    //          SDL SET UP SECTION
    //=======================================
    SDL_Init(SDL_INIT_VIDEO);

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

    // Create the window and OpenGL context
    SDL_Window* window =
        SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH,
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
    nk_set_style(ctx, THEME_WHITE);

    //=======================================
    //          OPENGL OBJECT SETUP
    //=======================================
    // clang-format off
    const GLfloat vertices[] = {

        -0.5f, -0.5f, 0.0f, 
         0.5f, -0.5f, 0.0f, 
         0.0f,  0.5f, 0.0f,
    };
    // clang-format on

    // Create triangle VAO
    GLuint vao = 0;
    GLuint vbo = 0;

    glCreateVertexArrays(1, &vao);
    glEnableVertexArrayAttrib(vao, 0);
    glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(vao, 0, 0);

    // Create VBO, attatch it to the VAO
    glCreateBuffers(1, &vbo);
    glNamedBufferStorage(vbo, sizeof(GLfloat) * 9, vertices, GL_DYNAMIC_STORAGE_BIT);
    glVertexArrayVertexBuffer(vao, 0, vbo, 0, 3 * sizeof(GLfloat));

    // Load shaders
    GLuint program =
        loadShaders("Data/Shaders/MinVertex.glsl", "Data/Shaders/MinFragment.glsl");
    glUseProgram(program);

    //=======================================
    //          OPENGL MISC SETUP
    //=======================================
    // glDisable(GL_DEPTH_TEST);
    glClearColor(0.5, 0.5, 0.5, 0.0);
    glViewport(0, 0, WIDTH, HEIGHT);

    //=======================================
    //          MAIN LOOP
    //=======================================
    bool running = true;
    while (running) {
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

                case SDL_QUIT:
                    running = false;
                    break;
            }
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        nk_overview(ctx);
        nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);

        glBindVertexArray(vao);
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(window);
    }

    //=======================================
    //          CLEAN UP
    //=======================================
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    glDeleteProgram(program);
    nk_sdl_shutdown();
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
