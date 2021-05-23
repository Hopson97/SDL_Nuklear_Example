#include "GUI.h"

#include <nuklear/nuklear_def.h>
#include <nuklear/nuklear_sdl_gl3.h>

#include "Camera.h"

#define MAX_VERTEX_MEMORY 0x80000
#define MAX_ELEMENT_MEMORY 0x20000

struct nk_context* ctx;
int start;
int frames = 0;
int fps = 0;

void guiInit(SDL_Window* window)
{
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

    start = SDL_GetTicks();
}

void guiShutdown(void)
{
    nk_sdl_shutdown();
}

void guiBeginFrame(void)
{
    nk_input_begin(ctx);
    frames++;
    int timeNow = SDL_GetTicks();
    if (timeNow - start >= 1000) {
        fps = frames;
        frames = 0;
        start = timeNow;
    }
}

void guiProcessEvent(SDL_Event* event)
{
    nk_sdl_handle_event(event);
}

void guiDebugScreen(struct Camera* c)
{
    if (nk_begin(ctx, "Debug Window", nk_rect(0, 0, 400, 200), 0)) {
        nk_layout_row_dynamic(ctx, 25, 1);
        nk_labelf(ctx, NK_STATIC, "Player Position: (%f %f %f)", c->position[0], c->position[1], c->position[2]);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_labelf(ctx, NK_STATIC, "Player Rotation: (%f %f %f)", c->rotation[0], c->rotation[1], c->rotation[2]);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_labelf(ctx, NK_STATIC, "Player Front: (%f %f %f)", c->front[0], c->front[1], c->front[2]);

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_labelf(ctx, NK_STATIC, "FPS: %d", fps);
    }
    nk_end(ctx);
}

void guiEndFrame(void)
{
    nk_sdl_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY, MAX_ELEMENT_MEMORY);
}
