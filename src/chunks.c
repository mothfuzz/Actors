#include "chunks.h"
#include "utils.h"
#include <string.h>
#include <stdio.h>

struct chunk *chunk_alloc(size_t item_size) {
    struct chunk *head = malloc(sizeof(struct chunk));
    //doesn't really matter
    //memset(head->data, 0, CHUNK_SIZE);
    head->item_size = item_size;
    head->len = 0;
    head->next = NULL;
    list_init(head->inserts.records);
    list_init(head->removes.records);
    return head;
}
void chunk_free(struct chunk *chunk) {
    if(chunk == NULL) {
        return;
    }
    chunk_free(chunk->next);
    foreach(i, &chunk->inserts.records) {
        struct inserts *ins = list_entry(i, struct inserts, records);
        list_free_entry(i, ins);
    }
    foreach(i, &chunk->removes.records) {
        struct removes *rem = list_entry(i, struct removes, records);
        list_free_entry(i, rem);
    }
    free(chunk);
}

void chunk_copy_data(struct chunk_addr *addr, void *data) {
    //add to inserts
    struct inserts *ins = malloc(sizeof(struct inserts));
    list_insert(&addr->owner->inserts.records, &ins->records);
    addr->pos = ins;
    ins->addr = *addr;
    //actually copy data
    memcpy(addr->owner->data + addr->offset, data, addr->owner->item_size);
}

struct chunk_addr chunk_insert(struct chunk *head, void *data) {
    /*
    ** okay so the structure of this function is gonna be generally
    ** 1. check this chunk's free list, if so, use that & return
    ** 2. check head->len to see if we have space for another item at the end
    ** 3. if not, then move to head->next and go back to 1.
    ** 3.b make sure head->next is initialized, if it's NULL then alloc it.
    ** 4. if we do have space, then add it at the end and increment head->len then return
    */

    foreach(i, &head->removes.records) {
        struct removes *rem = list_entry(i, struct removes, records);
        struct chunk_addr addr = rem->addr;
        list_free_entry(i, rem);
        chunk_copy_data(&addr, data);
        return addr;
    }

    if((head->len + 1) * head->item_size >= CHUNK_SIZE) {
        if(!head->next) {
            head->next = chunk_alloc(head->item_size);
            printf("allocated new chunk: %p\n", head->next);
            fflush(stdout);
        }
        return chunk_insert(head->next, data);
    }

    struct chunk_addr addr = {head, head->len*head->item_size};
    chunk_copy_data(&addr, data);
    head->len++;
    return addr;
}

//warning: this does not check validity of the address.
//if you want only valid addresses, use the iterator.
void *chunk_get(struct chunk_addr addr) {
    if(addr.owner) {
        return &addr.owner->data[addr.offset];
    } else {
        return NULL;
    }
}

void chunk_remove(struct chunk_addr *addr) {
    //get next insert
    struct inserts *next = list_entry(addr->pos->records.next, struct inserts, records);

    //remove from inserts
    list_free_entry(&addr->pos->records, addr->pos);

    //add to removes
    struct removes *rem = malloc(sizeof(struct removes));
    rem->addr = *addr;
    list_insert(&addr->owner->removes.records, &rem->records);

    //restore cursor incase this is an iterator
    addr->pos = list_entry(next->records.prev, struct inserts, records);
}

struct chunk_addr chunk_iter_start(struct chunk *c) {
    foreach(i, &c->inserts.records) {
        struct inserts *ins = list_entry(i, struct inserts, records);
        return ins->addr;
    }
    return (struct chunk_addr){NULL, 0};
}
struct chunk_addr chunk_iter_next(struct chunk_addr *iter) {
    /*
    ** 1. iterate through the inserts list of the current chunk
    ** 2. if that's done, check and see if there's a next chunk
    ** 3. if so, then continue through iter->owner->next
    ** 4. if not, then return a NULL addr
        */
    if(iter->owner == NULL) {
        return (struct chunk_addr){NULL, 0};
    }
    if(iter->pos->records.next != &iter->owner->inserts.records) {
        struct inserts *next = list_entry(iter->pos->records.next, struct inserts, records);
        iter->offset = next->addr.offset;
        iter->pos = next->addr.pos;
        return next->addr;
    } else {
        if(iter->owner->next) {
            struct chunk_addr next_start = chunk_iter_start(iter->owner->next);
            *iter = next_start;
            return next_start;
        } else {
            return (struct chunk_addr){NULL, 0};
        }
    }
}
