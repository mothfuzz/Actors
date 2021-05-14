#ifndef __EVENT_TYPES_H_
#define __EVENT_TYPES_H_

//this file should be edited by the user til I find a better way to do serial ids per type in C.

enum event_types {
EVENT_TYPE_KILL,
EVENT_TYPE_PARENT_DIED,
EVENT_TYPE_COLLISION,
EVENT_TYPE_DAMAGE,
EVENT_TYPES_MAX,
};

struct damage {
    int amount;
};

struct collision {
    int other_id;
};


#endif // __EVENT_TYPES_H_
