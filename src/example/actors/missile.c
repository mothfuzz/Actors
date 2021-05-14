#include "missile.h"
#include "events.h"

#include "player.h"

#include <math.h>

float rand_max(float max) {
    return (float)rand()/(float)RAND_MAX * max;
}

void init_missile(void *v) {
    struct missile *m = v;
    m->transform = init_transform(m->id);
    m->sprite = init_sprite_texture(m->id, "../resources/missile.png");
    m->transform->rotation.z = rand_max(360);
    int side = rand_max(3);
    if(side == 0) {
        m->transform->position.x = rand_max(640);
        m->transform->position.y = 0.0;
    } else if(side == 1) {
        m->transform->position.x = 0.0;
        m->transform->position.y = rand_max(400);
    } else if(side == 2) {
        m->transform->position.x = 640.0;
        m->transform->position.y = rand_max(400);
    } else {
        m->transform->position.x = rand_max(640);
        m->transform->position.x = 400.0;
    }
    init_event_queue(m->id, &m->collisions, EVENT_TYPE_COLLISION, sizeof(struct collision));
}
int update_missile(void *v) {
    struct missile *m = v;
    struct vec3 *pos = &m->transform->position;
    pos->x += cos(m->transform->rotation.z * acos(-1) / 180.0) * 3.0;
    pos->y += sin(m->transform->rotation.z * acos(-1) / 180.0) * 3.0;
    if(pos->x > 640 || pos->x < 0 || pos->y > 400 || pos->y < 0) {
        spawn(&missile_spawner);
        return 1;
    }

    struct collision *col;
    while((col = next_event(&m->collisions))) {
        if(col->other_id == PLAYER_ID) {
            //boom! ouch.
            dispatch(PLAYER_ID, EVENT_TYPE_DAMAGE, &(struct damage){10});
            spawn(&missile_spawner);
            return 1;
        }
    }
    return 0;
}
