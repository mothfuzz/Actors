ACTORS

This is a proof-of-concept game-oriented actor system implemented in C.

The idea behind this engine was to leverage the strengths of ECS for data storage, and the strengths of traditional game-object-systems for game logic and program flow.

The strength of actors is that they are completely isolated units of logic - in order to know how the player works, for example, you only need to check `player.c`. Same with any other in-game object.
Actors should not manipulate each other directly, and instead opt to send Events to each other via `dispatch`
and `broadcast`. The engine takes care of the rest.

Finally, actors can subscribe and unsubscribe to Services, which represent basic engine functionality & are backed by cache-efficient mass storage courtesy of the `chunk` data structure. The idea of chunking a bunch of data together and then only using entity IDs to reference it should be familiar to those familiar with ECS. Except in this case, addresses are stable so you *can* use a pointer if you really want.


Features include but are not limited to:
- Actors, which are individual game entities
- Events, which can be sent to a specific actor via `dispatch` or to all actors via `broadcast`
- Services, which are plugins that provide engine functionality that actors can subscribe and unsubscribe to.
- automatic clean-up for events, event queues, and services - no need to call 'free' in actor code.
- chunks for mass data storage of any type, with stable addresses so that their data can be pointed to by actors
- bonus: intrusive lists, arrays, and hashmaps all in `utils.c`

It's pretty fast. Not the fastest in the world but there are many areas of improvement.
My tests without collision detection & no multithreading are able to run up to around 70k actors at 60fps.
With multithreading & proper synchronization I could make it much MUCH faster.
With a really bad O(n^2) collision detection algorithm I can get between 500-1000 actors. Blame the algorithm.

How to implement:

An Actor:

``` c
struct my_actor {
    //very first field must be an int representing the actor's ID
    int id;
    //after the ID, you can have custom data of any size
    int a;
    float b;
    double c;
    //you can have pointers to types you get from services, they will be valid as long as the actor itself is.
    struct transform *t;
    struct sprite *s;
    //you can have a number of event queues of any type
    struct event_queue collisions;
};

void init_my_actor(void *a) {
    //init_actor takes a void*
    //the ID will have been set by the engine at this point.
    struct my_actor *m = a;
    m->a = 3;
    m->b = 4.5f;
    m->c = 6.8;
    //these two init functions are provided by other engine services
    //t & s will automatically be cleaned up upon actor death
    m->t = init_transform(m->id);
    m->s = init_sprite_texture(m->id, "../resources/actor.png");
    //this sets up the event queue to a particular type i.e. a particular channel
    //this will also be automatically cleaned up
    init_event_queue(m->id, &m->collisions, EVENT_TYPE_COLLISION, sizeof(struct collision));
}

int update_my_actor(void *a) {
    struct my_actor *m = a;
    
    //you can iterate through events using next_event, which returns NULL when the queue is empty.
    struct collision *c;
    while((c = next_event(&m->collisions))) {
        printf("I collided with entity: %d\n", c->other_id);
    }
    
    //return 0 for normal, or return non-zero for the actor to be destroyed automatically.
    return 0;
}

//actors must have an init_* function, though an update is not required. Place them in a spawner:
struct spawner my_actor_spawner = {sizeof(struct my_actor), init_my_actor, update_my_actor}

//then you can spawn a new my_actor using spawn:
struct my_actor *m = spawn(&my_actor);

```

An Event:

``` c
//Put a struct for your event type somewhere visible:
struct my_event {
    int value;
};

//then add it's type to the event_types enum in event_types.h:
enum event_types {
    EVENT_TYPE_MY_EVENT,
    //make sure EVENT_TYPES_MAX is the last one as that is used internally.
    EVENT_TYPES_MAX,
};

//then you can call:
dispatch(some_actor_id, EVENT_TYPE_MY_EVENT, &(struct my_event){5});
broadcast(EVENT_TYPE_MY_EVENT, &(struct my_event){5});

```

A Service:

``` c
//Services must have the following 5 functions in their defintion:
//a subscribe function which returns the services' internal data type
//an unsubscribe function
//(optionally) an update function
//an initialization function for the whole service
//a shutdown function for the whole service

struct service my_service = {
    my_service_subscribe, 
    my_service_unsubscrube, 
    my_service_update, 
    init_my_service, 
    destroy_my_service
};

//services are associated with a particular data type which the Actor has a reference to. 
//For the renderer it might be a sprite or model. For physics it might be a rigidbody. Etc. Etc.
struct my_service_item {
    int value;
};

//usually these values are kept in a chunk or indexed_chunk:
struct indexed_chunk my_service_items;

//if you use indexed_chunk these functions will all look relatively the same 
//no matter what the service actually does.
//hooray for reusability
void *my_service_subscribe(int id) {
    //An actor will set these values to whatever it wants, so keep sane defaults
    struct my_service_item default_item = {0}; 
    return indexed_chunk_insert(&my_service_items, id, &default_item);
}

void my_service_unsubscribe(int id) {
    indexed_chunk_remove(&my_service_items, id);
}

void my_service_update() {
    //iteration should be fast since it's packed in one big block of memory.
    indexed_chunk_foreach(i, id, &my_service_items) {
        struct my_service_item *item = indexed_chunk_get(&my_service_items, id);
        //do something with item
        item->value++;
    }
}

void init_my_service() {
    indexed_chunk_init(&my_service_items, sizeof(struct my_service_item));
}
void destroy_my_service() {
    indexed_chunk_free(&my_service_items);
}

//finally, somewhere before the service is used (usually in main) call:
register_service(&my_service);
```

