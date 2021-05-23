#include "Utility.h"
#include "Graphics/GLDebug.h"
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>

char* getFileContent(const char* fileName)
{
    char* buffer = NULL;
    long length = 0;
    FILE* file = fopen(fileName, "r");

    if (file) {
        fseek(file, 0, SEEK_END);
        length = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer = calloc(length + 1, 1);
        if (buffer) {
            fread(buffer, 1, length, file);
        }
        fclose(file);
    }
    return buffer;
}

static int isColourSame(struct Colour a, struct Colour b)
{
    return a.red == b.red && a.green == b.green && a.blue == b.blue;
}

static void setTerminalColour(struct Colour colour, enum ColourSetMode mode)
{
    static int isFirstTimeBG = 1;
    static int isFirstTimeFG = 1;
    static struct Colour currentTextColour;
    static struct Colour currentBackgroundColour;
    uint8_t r = colour.red;
    uint8_t g = colour.green;
    uint8_t b = colour.blue;
    switch (mode) {
        case COL_SET_BG:
            if (!isColourSame(currentBackgroundColour, colour) || isFirstTimeBG) {
                printf("\x1B[%d;2;%d;%d;%dm", mode, r, g, b);
                isFirstTimeBG = 1;
            }
            break;

        case COL_SET_FG:
            if (!isColourSame(currentTextColour, colour) || isFirstTimeFG) {
                printf("\x1B[%d;2;%d;%d;%dm", mode, r, g, b);
                isFirstTimeFG = 1;
            }
            break;
    }
}

void setBackgroundColour(struct Colour colour)
{
    setTerminalColour(colour, COL_SET_BG);
}

void setTextColour(struct Colour colour)
{
    setTerminalColour(colour, COL_SET_FG);
}

void setTextColourRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    struct Colour colour = {
        .red = red,
        .green = green,
        .blue = blue,
    };
    setTerminalColour(colour, COL_SET_FG);
}

void setBackgroundColourRGB(uint8_t red, uint8_t green, uint8_t blue)
{
    struct Colour colour = {
        .red = red,
        .green = green,
        .blue = blue,
    };
    setTerminalColour(colour, COL_SET_BG);
}

SDL_Window* initWindow()
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

    SDL_Window* window = SDL_CreateWindow("OpenGL Nuklear Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    SDL_GL_CreateContext(window);
    if (!gladLoadGL()) {
        printf("Error: Could not load OpenGL.");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }
    initGLDebug();
    return window;
}
