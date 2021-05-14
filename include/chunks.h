#ifndef __CHUNKS_H_
#define __CHUNKS_H_

#include "utils.h"

struct chunk_addr {
    struct chunk *owner; //which chunk
    int offset; //how far into chunk->data, in bytes
    struct inserts *pos; //where in inserts list it's kept
};

//keep records of inserts and deletes for optimal performance / space usage
struct inserts {
    struct chunk_addr addr;
    struct list records;
};

struct removes {
    struct chunk_addr addr;
    struct list records;
};


#define CHUNK_SIZE 1024*16

//16kb chunks for cache coherency
struct chunk {
    byte data[CHUNK_SIZE];
    size_t item_size;
    int len; //when nothing in free list, simply append to end
    struct chunk *next;
    struct inserts inserts; //we can iterate this quickly
    struct removes removes; //we can reclaim memory from this
};

struct chunk *chunk_alloc(size_t item_size);
void chunk_free(struct chunk *chunk);

struct chunk_addr chunk_insert(struct chunk *head, void *data); //will have to be recursive since they're connected
void *chunk_get(struct chunk_addr addr);
void chunk_remove(struct chunk_addr *addr);

struct chunk_addr chunk_iter_start(struct chunk *chunk);
struct chunk_addr chunk_iter_next(struct chunk_addr *cursor);

#define chunk_foreach(i, chunk)                                         \
    for(struct chunk_addr i = chunk_iter_start(chunk); i.owner; i = chunk_iter_next(&i))

#define chunk_foreach_entry(type, i, chunk)                             \
    struct chunk_addr iter = chunk_iter_start(chunk);                   \
    for(type *i = chunk_get(iter); i; i = chunk_get(chunk_iter_next(&iter)))

#endif // __CHUNKS_H_
