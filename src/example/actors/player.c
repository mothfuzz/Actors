#include "player.h"

#include <SDL_render.h>
#include <stdio.h>
#include <SDL2/SDL.h>

#include "../services/transform.h"
#include "../services/renderer.h"

int PLAYER_ID = -1;

void init_healthbar(void *v) {
    struct healthbar *h = v;
    h->sprite = init_sprite_texture(h->id, "../resources/health.png");
    h->sprite->w = 100;
}

int check_key(const char *name) {
    SDL_Scancode scancode = SDL_GetScancodeFromName(name);
    if(scancode != SDL_SCANCODE_UNKNOWN) {
        const Uint8 *keystate = SDL_GetKeyboardState(NULL);
        return (int)keystate[scancode];
    } else {
        return 0;
    }
}

void init_player(void *v) {
    struct player *p = v;
    PLAYER_ID = p->id;
    p->health = 100;
    p->transform = init_transform(p->id);
    p->transform->position.x = 640.0 / 2.0;
    p->transform->position.y = 400.0 / 2.0;
    p->sprite = init_sprite_texture(p->id, "../resources/player.png");
    init_event_queue(p->id, &p->damage, EVENT_TYPE_DAMAGE, sizeof(struct damage));

    p->healthbar = spawn(&healthbar_spawner);
}

int update_player(void *v) {
    struct player *p = v;

    if(p->health <= 0) {
        printf("I died!!\n");
        fflush(stdout);
        kill(p->healthbar->id);
        PLAYER_ID = -1;
        return 1;
    }

    if(check_key("down")) {
        dispatch(PLAYER_ID, EVENT_TYPE_DAMAGE, &(struct damage){100});
    }

    struct vec3 *position = &p->transform->position;
    float *rotation = &p->transform->rotation.z;
    if(check_key("w")) {
        position->y -= 4.0;
        *rotation = 270;
    }
    if(check_key("s")) {
        position->y += 4.0;
        *rotation = 90;
    }
    if(check_key("a")) {
        position->x -= 4.0;
        *rotation = 180;
    }
    if(check_key("d")) {
        position->x += 4.0;
        *rotation = 0;
    }

    struct damage *d;
    while((d = next_event(&p->damage))) {
        printf("I was damaged!!! hp -= %d\n", d->amount);
        fflush(stdout);
        p->health -= d->amount;
        p->healthbar->sprite->w = p->health;
    }

    return 0;
}
