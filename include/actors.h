#ifndef __ACTORS_H_
#define __ACTORS_H_

struct spawner {
    size_t size;
    void (*init_func)(void*);
    int (*update_func)(void*);
    /*all actors should be freeable by free() so no free_func*/
};

void *spawn(struct spawner *spawner);
void tick();
void kill(int id); //note: inefficient, consider an EVENT_TYPE_KILL handler instead
void destroy_the_world();


#endif // __ACTORS_H_
