#include "services.h"

#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

struct registered_service {
    struct service *service;
    struct list links;
} *registered_services = NULL;

void register_service(struct service *s) {
    if(!registered_services) {
        registered_services = malloc(sizeof(struct registered_service));
        list_init(registered_services->links);
    }
    s->init_service();
    struct registered_service *rs = malloc(sizeof(struct registered_service));
    rs->service = s;
    list_insert(&registered_services->links, &rs->links);
}

void update_all_services() {
    if(registered_services) {
        foreach(i, &registered_services->links) {
            struct registered_service *rs = list_entry(i, struct registered_service, links);
            if(rs->service->update_all_actors) {
                rs->service->update_all_actors();
            }
        }
    }
}
void actor_unsubscribe_all(int id) {
    if(registered_services) {
        foreach(i, &registered_services->links) {
            struct registered_service *rs = list_entry(i, struct registered_service, links);
            rs->service->actor_unsubscribe(id);
        }
    }
}

void destroy_all_services() {
    if(registered_services) {
        foreach(i, &registered_services->links) {
            struct registered_service *rs = list_entry(i, struct registered_service, links);
            rs->service->destroy_service();
            list_free_entry(&rs->links, rs);
        }
    }
}
