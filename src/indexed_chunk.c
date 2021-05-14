#include "indexed_chunk.h"
#include "chunks.h"
#include "utils.h"
#include <stdio.h>

void indexed_chunk_init(struct indexed_chunk *chunk, size_t data_size) {
    chunk->chunk = chunk_alloc(data_size);
    hash_init(&chunk->entries, sizeof(struct chunk_addr));
}
void indexed_chunk_free(struct indexed_chunk *chunk) {
    chunk_free(chunk->chunk);
    hash_free(&chunk->entries);
}

void *indexed_chunk_insert(struct indexed_chunk *chunk, int id, void *default_data) {
    struct chunk_addr addr = chunk_insert(chunk->chunk, default_data);
    hash_insert(&chunk->entries, id, &addr);
    return chunk_get(addr);
}
void indexed_chunk_remove(struct indexed_chunk *chunk, int id) {
    struct chunk_addr *addr;
    if((addr = hash_get(&chunk->entries, id))) {
        chunk_remove(addr);
        hash_remove(&chunk->entries, id);
    }
}
void *indexed_chunk_get(struct indexed_chunk *chunk, int id) {
    struct chunk_addr *addr;
    if((addr = hash_get(&chunk->entries, id))) {
        return chunk_get(*addr);
    } else {
        return NULL;
    }
}
