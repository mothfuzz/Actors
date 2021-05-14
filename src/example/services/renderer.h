#ifndef __RENDERER_H_
#define __RENDERER_H_

#include <SDL2/SDL.h>
#include "services.h"

struct sprite {
    SDL_Texture *texture;
    int w, h;
    int flip; //0 = no flip; 1 = fliph; 2 = flipv; 3 = both
};

void *init_sprite(int id);
void destroy_sprite(int id);
void sprite_set_texture(struct sprite *spr, const char *filename);
struct sprite *init_sprite_texture(int id, const char *filename);

void init_renderer();
void destroy_renderer();
void render();

static struct service renderer_service = {init_sprite, destroy_sprite, render, init_renderer, destroy_renderer};

#endif // __RENDERER_H_
