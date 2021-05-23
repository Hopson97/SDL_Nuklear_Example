#pragma once

#include <SDL2/SDL.h>

struct Camera;

void guiInit(SDL_Window* window);
void guiShutdown(void);

void guiBeginFrame(void);
void guiProcessEvent(SDL_Event* event);
void guiEndFrame(void);

void guiDebugScreen(struct Camera* camera);
