#ifndef __SERVICES_H_
#define __SERVICES_H_

#include "utils.h"

struct service {
    //size_t data_size;
    void *(*actor_subscribe)(int id);
    void (*actor_unsubscribe)(int id);
    void (*update_all_actors)();
    void (*init_service)();
    void (*destroy_service)();
};

void register_service(struct service *s);
void update_all_services();
void actor_unsubscribe_all(int id);
void destroy_all_services();

#endif // __SERVICES_H_
