#ifndef __TRANSFORM_H_
#define __TRANSFORM_H_

#include "utils.h"
#include "services.h"

struct transform {
    struct vec3 position;
    struct vec3 rotation;
    struct vec3 scale;
};

//associates an id with a new transform
void *init_transform(int id);
//destroys all transforms associated with id
void destroy_transform(int id);

void init_transform_service();
void destroy_transform_service();

static struct service transform_service = {init_transform, destroy_transform, NULL, init_transform_service, destroy_transform_service};

struct transform *get_transform(int id);

#endif // __TRANSFORM_H_
