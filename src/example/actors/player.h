#ifndef __PLAYER_H_
#define __PLAYER_H_

#include "actors.h"
#include "events.h"

extern int PLAYER_ID;

struct healthbar {
    int id;
    struct sprite *sprite;
};
void init_healthbar(void *v);
static struct spawner healthbar_spawner = {sizeof(struct healthbar), init_healthbar, NULL};

//damage events the player receives
//tagged by EVENT_TYPE_DAMAGE
struct player {
    int id;
    struct transform *transform;
    //struct boundingbox *bb;
    struct sprite *sprite;
    int health;
    struct healthbar *healthbar;
    struct event_queue damage;
};

void init_player(void *v);
int update_player(void *v);

static struct spawner player_spawner = {sizeof(struct player), init_player, update_player};

#endif // __PLAYER_H_
