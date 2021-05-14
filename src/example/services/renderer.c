#include "renderer.h"
#include "chunks.h"
#include "event_types.h"
#include "indexed_chunk.h"
#include "utils.h"
#include "transform.h"
#include "events.h"

#include <SDL2/SDL_image.h>
#include <SDL_render.h>
#include <SDL_surface.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static struct indexed_chunk sprites;
int ticks = 0;
int frames = 0;

static SDL_Texture *player_texture = NULL;

void *init_sprite(int id) {
    struct sprite def = {NULL, 16, 16, 0};
    return indexed_chunk_insert(&sprites, id, &def);
}
void destroy_sprite(int id) {
    struct sprite *spr = indexed_chunk_get(&sprites, id);
    SDL_DestroyTexture(spr->texture);
    indexed_chunk_remove(&sprites, id);
}
void sprite_set_texture(struct sprite *spr, const char *filename) {
    SDL_Surface *s = IMG_Load(filename);
    SDL_SetColorKey(s, 1, SDL_MapRGB(s->format, 255, 0, 255));
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
    spr->texture = tex;
}
struct sprite *init_sprite_texture(int id, const char *filename) {
    struct sprite *s = init_sprite(id);
    sprite_set_texture(s, filename);
    int w, h;
    SDL_QueryTexture(s->texture, NULL, NULL, &w, &h);
    s->w = w;
    s->h = h;
    return s;
}

void init_renderer() {
    window = SDL_CreateWindow("hewwo!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 400, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    indexed_chunk_init(&sprites, sizeof(struct sprite));

    SDL_Surface *s = IMG_Load("../resources/player.png");
    SDL_SetColorKey(s, 1, SDL_MapRGB(s->format, 255, 0, 255));
    player_texture = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FreeSurface(s);
}
void destroy_renderer() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    indexed_chunk_free(&sprites);
}

void update_collision() {
    indexed_chunk_foreach(i, id_a, &sprites) {
        struct sprite *spr_a = indexed_chunk_get(&sprites, id_a);
        struct transform *t_a = get_transform(id_a);
        if(t_a) {
            indexed_chunk_foreach(j, id_b, &sprites) {
                struct sprite *spr_b = indexed_chunk_get(&sprites, id_b);
                struct transform *t_b = get_transform(id_b);
                if(t_b) {
                    SDL_Rect a = {t_a->position.x, t_a->position.y, spr_a->w, spr_a->h};
                    SDL_Rect b = {t_b->position.x, t_b->position.y, spr_b->w, spr_b->h};
                    if(a.x + a.w > b.x && a.x < b.x + b.w
                       && a.y + a.h > b.y && a.y < b.y + b.h) {
                        struct collision col_a = {id_b};
                        struct collision col_b = {id_a};
                        dispatch(id_a, EVENT_TYPE_COLLISION, &col_a);
                        dispatch(id_b, EVENT_TYPE_COLLISION, &col_b);
                    }
                }
            }
        }
    }

}

void render() {
    SDL_SetRenderDrawColor(renderer, 128, 200, 255, 255);
    SDL_RenderClear(renderer);

    indexed_chunk_foreach(i, id, &sprites) {
        struct sprite *spr = indexed_chunk_get(&sprites, id);
        struct transform *t = get_transform(id);
        SDL_Rect dst = {0, 0, spr->w, spr->h};
        float rotation = 0.0;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        if(spr->flip & 0x1) {
            flip |= SDL_FLIP_HORIZONTAL;
        }
        if(spr->flip & 0x10) {
            flip |= SDL_FLIP_VERTICAL;
        }
        if(t) {
            dst.x = t->position.x;
            dst.y = t->position.y;
            rotation = t->rotation.z;
        }
        SDL_RenderCopyEx(renderer, spr->texture, NULL, &dst, rotation, NULL, flip);
        //SDL_RenderCopyEx(renderer, player_texture, NULL, &dst, rotation, NULL, flip);
    }

    SDL_RenderPresent(renderer);

    int now = SDL_GetTicks();
    if(now > ticks + 1000) {
        char title[16];
        sprintf(title, "FPS: %d", frames);
        SDL_SetWindowTitle(window, title);
        frames = 0;
        ticks = now;
    }
    frames++;

    update_collision();
}

