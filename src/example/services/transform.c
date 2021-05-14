#include "transform.h"

#include <stdlib.h>
#include <stdio.h>

#include "indexed_chunk.h"

static struct indexed_chunk transforms;

void init_transform_service() {
    indexed_chunk_init(&transforms, sizeof(struct transform));
}
void destroy_transform_service() {
    indexed_chunk_free(&transforms);
}

void* init_transform(int id) {
    struct transform def = {{0, 0, 0}, {0, 0, 0}, {1, 1, 1}};
    return indexed_chunk_insert(&transforms, id, &def);
}
void destroy_transform(int id) {
    indexed_chunk_remove(&transforms, id);
}

struct transform *get_transform(int id) {
    return indexed_chunk_get(&transforms, id);
}
