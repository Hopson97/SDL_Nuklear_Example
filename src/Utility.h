#pragma once

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>

// Default window height and width
#define WIDTH 1600
#define HEIGHT 900

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

char* getFileContent(const char* fileName);

// Colour Util
struct Colour {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

enum ColourSetMode {
    COL_SET_BG = 48,
    COL_SET_FG = 38,
};

void setTerminalBackgroundColour(struct Colour colour);
void setTerminalTextColour(struct Colour colour);

void setBackgroundColourRGB(uint8_t red, uint8_t green, uint8_t blue);
void setTextColourRGB(uint8_t red, uint8_t green, uint8_t blue);

// SDL Wrapper stuff
SDL_Window* initWindow();
