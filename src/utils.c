#include "utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void list_insert(struct list *list, struct list *item) {
    item->prev = list->prev;
    item->next = list;
    list->prev->next = item;
    list->prev = item;
}
void list_remove(struct list *item) {
    item->prev->next = item->next;
    item->next->prev = item->prev;
}
void list_free_entry(struct list *list, void *entry) {
    //splice out the list
    list_remove(list);
    //free the item's memory
    free(entry);
}

void array_init(struct array *a, size_t item_size) {
    a->data = NULL;
    a->item_size = item_size;
    a->len = 0;
    a->cap = 1;
}
void array_push(struct array *a, void *data) {
    a->len+=1;
    if(a->len >= a->cap) {
        a->cap *= 2;
        a->data = realloc(a->data, a->cap * a->item_size);
    }

    //printf("new len: %d\n", a->len);
    //printf("new cap: %d\n", a->cap);
    //fflush(stdout);

    void *back = array_get(a, a->len - 1);
    memcpy(back, data, a->item_size);
}
void *array_pop(struct array *a) {
    if(a->len > 0) {
        a->len--;
        return array_get(a, a->len);
    } else {
        return NULL;
    }
}
void *array_get(struct array *a, int i) {
    return (byte*)(a->data) + i*a->item_size;
}
void array_swap_remove(struct array *a, int *i) {
    a->len--;
    memcpy(array_get(a, *i), array_get(a, a->len), a->item_size);
    *i-=1; //restore cursor in case i is iterating. don't skip swapped value
}
void array_free(struct array *a) {
    free(a->data);
}


int hash_func(struct hash *h, int key) {
    //it's fuckin nothign
    int hash = key % h->cap;
    return hash;
}
void hash_init(struct hash *h, size_t value_size) {
    h->item_size = value_size;
    h->len = 0;
    h->cap = 16;
    h->keys = malloc(sizeof(int) * h->cap);
    h->values = malloc(h->item_size * h->cap);
    h->occupieds = malloc(sizeof(int) * h->cap);
    memset(h->occupieds, 0, sizeof(int) * h->cap);
}
void rehash(struct hash *h) {
    struct hash new_h;
    new_h.item_size = h->item_size;
    new_h.len = h->len;
    new_h.cap = h->cap * 2;
    new_h.keys = malloc(sizeof(int) * new_h.cap);
    new_h.values = malloc(new_h.item_size * new_h.cap);
    new_h.occupieds = malloc(sizeof(int) * new_h.cap);
    memset(new_h.occupieds, 0, sizeof(int) * new_h.cap);
    for(int i = 0; i < h->cap; ++i) {
        if(h->occupieds[i]) {
            hash_insert(&new_h, h->keys[i], h->values + i * h->item_size);
        }
    }
    hash_free(h);
    *h = new_h;
}
void hash_insert(struct hash *h, int key, void *value) {
    h->len++;
    if(h->len > (float)(h->cap) * 0.75) {
        rehash(h);
    }
    int hash = hash_func(h, key);
    while(h->occupieds[hash]) {
        hash = (hash + 1) % h->cap;
    }
    h->keys[hash] = key;
    memcpy(h->values + hash*h->item_size, value, h->item_size);
    h->occupieds[hash] = 1;
}
void *hash_get(struct hash *h, int key) {
    int hash = hash_func(h, key);
    while(h->keys[hash] != key && h->occupieds[hash]) {
        hash = (hash + 1) % h->cap;
    }
    if(h->occupieds[hash]) {
        return h->values + hash * h->item_size;
    } else {
        return NULL;
    }
}
void hash_remove(struct hash *h, int key) {
    int hash = hash_func(h, key);
    while(h->keys[hash] != key && h->occupieds[hash]) {
        hash = (hash + 1) % h->cap;
    }
    if(h->occupieds[hash]) {
        h->occupieds[hash] = 0;
    }
}
void hash_free(struct hash *h) {
    free(h->keys);
    free(h->values);
    free(h->occupieds);
}

int *hash_next(struct hash *h, int *key_iter) {
    int i;
    if(!key_iter) {
        i = 0;
    } else {
        i = key_iter - h->keys + 1;
    }
    while(i < h->cap && !h->occupieds[i]) {
        i++;
    }
    if(i >= h->cap) {
        return NULL;
    }
    return h->keys + i;
}
