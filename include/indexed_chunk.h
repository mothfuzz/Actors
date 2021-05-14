#ifndef __CHUNK_INDEX_H_
#define __CHUNK_INDEX_H_

#include "utils.h"
#include "chunks.h"

struct indexed_chunk {
    struct chunk *chunk;
    struct hash entries;
};

void indexed_chunk_init(struct indexed_chunk *chunk, size_t data_size);
void indexed_chunk_free(struct indexed_chunk *chunk);
void *indexed_chunk_insert(struct indexed_chunk *chunk, int id, void *default_data);
void indexed_chunk_remove(struct indexed_chunk *chunk, int id);
void *indexed_chunk_get(struct indexed_chunk *chunk, int id);

inline int next_occupied(struct indexed_chunk *chunk, int i) {
    do {
        i++;
    }
    while(!chunk->entries.occupieds[i] && i < chunk->entries.cap);
    return i;
}

#define indexed_chunk_foreach(i, id, chunk)                             \
    for(int i = 0, id = (chunk)->entries.keys[0];                       \
        (i < (chunk)->entries.cap) && (chunk)->entries.occupieds[i];    \
        i = next_occupied(chunk, i), id = (chunk)->entries.keys[i])

#endif // __CHUNK_INDEX_H_
