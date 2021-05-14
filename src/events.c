#include "events.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static struct hash handlers[EVENT_TYPES_MAX]; //event_type->id->event_handler

int is_handler_initialized(int event_type) {
    return handlers[event_type].item_size > 0;
}

void init_event_queue(int id, struct event_queue *e, int event_type, size_t data_size) {
    if(!is_handler_initialized(event_type)){
        hash_init(&handlers[event_type], sizeof(struct event_queue*));
    }

    array_init(&e->events, data_size);

    //yo fuckin uh put this pointer somewhere so we can dispatch to it.
    hash_insert(&handlers[event_type], id, &e);
}
void *next_event(struct event_queue *e) {
    return array_pop(&e->events);
}


void destroy_all_queues(int id) {
    for(int e = 0; e < EVENT_TYPES_MAX; e++) {
        if(is_handler_initialized(e)) {
            struct event_queue **q;
            if((q = hash_get(&handlers[e], id))) {
                hash_remove(&handlers[e], id);
            }
        }
    }
}

void dispatch(int id, int event_type, void *event) {
    if(is_handler_initialized(event_type)) {
        struct event_queue **q;
        if((q = hash_get(&handlers[event_type], id))) {
            array_push(&(*q)->events, event);
        }
    }
}


void broadcast(int event_type, void *event) {
    if(is_handler_initialized(event_type)) {
        struct hash *h = &handlers[event_type];
        int *id = NULL;
        while((id = hash_next(h, id))) {
            struct event_queue **q = hash_get(h, *id);
            array_push(&(*q)->events, event);
        }
    }
}
