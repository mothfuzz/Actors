#include "actors.h"

#include <stdlib.h>

#include "utils.h"
#include "events.h"
#include "services.h"

struct actor {
    void *data;
    int (*update)(void*);
    struct list actors;
};

list_new(all_actors);

/* don't */
#define ACTOR_ID(data) *((int*)(data))

static int actors_len = 0;
void *spawn(struct spawner *spawner) {
    int id = actors_len;
    actors_len++;
    void* new_data = malloc(spawner->size);
    ACTOR_ID(new_data) = id;
    if(spawner->init_func) {
        spawner->init_func(new_data);
    }

    struct actor *new_actor = malloc(sizeof(struct actor));
    new_actor->data = new_data;
    new_actor->update = spawner->update_func;

    list_insert(&all_actors, &new_actor->actors);

    return new_actor->data;
}

void delete_actor(struct list *i, struct actor *a) {
    //free all event queues
    destroy_all_queues(ACTOR_ID(a->data));
    //deregister from all services
    actor_unsubscribe_all(ACTOR_ID(a->data));
    //free actor itself
    free(a->data);
    list_free_entry(i, a);
}

void tick() {
    foreach(i, &all_actors) {
        struct actor *a = list_entry(i, struct actor, actors);
        if(a->update) {
            if(a->update(a->data) != 0) {
                delete_actor(i, a);
            }
        }
    }
    update_all_services();
}

void kill(int id) {
    foreach(i, &all_actors) {
        struct actor *a = list_entry(i, struct actor, actors);
        if(ACTOR_ID(a->data) == id) {
            delete_actor(i, a);
        }
    }
}

void destroy_the_world() {
    foreach(i, &all_actors) {
        struct actor *a = list_entry(i, struct actor, actors);
        delete_actor(i, a);
    }
    destroy_all_services();
}
