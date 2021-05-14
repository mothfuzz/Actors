#ifndef __MISSILE_H_
#define __MISSILE_H_

#include "actors.h"
#include "events.h"

#include "../services/transform.h"
#include "../services/renderer.h"

struct missile {
    int id;
    struct transform *transform;
    struct sprite *sprite;
    struct event_queue collisions;
};

void init_missile(void *v);
int update_missile(void *v);

static struct spawner missile_spawner = {sizeof(struct missile), init_missile, update_missile};


#endif // __MISSILE_H_
