#ifndef __EVENTS_H_
#define __EVENTS_H_

#include "event_types.h"
#include "utils.h"

//really just a byte buffer
struct event_queue {
    struct array events;
};

void init_event_queue(int id, struct event_queue *e, int event_type, size_t data_size);
void *next_event(struct event_queue *e);
void destroy_all_queues(int id);

void dispatch(int id, int event_type, void *event);
void broadcast(int event_type, void *event);


#endif // __EVENTS_H_
