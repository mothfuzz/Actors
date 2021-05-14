#ifndef __UTILS_H_
#define __UTILS_H_

#include <stdlib.h>

typedef unsigned char byte;

struct vec3 {
    float x, y, z;
};

struct list {
    struct list *next, *prev;
};
#define list_init(listv) listv = (struct list){&listv, &listv}
#define list_new(listv) struct list listv = {&listv, &listv}
#define list_entry(listp, type, member) (type*)((char*)(listp) - offsetof(type, member))
//if only the world were so simple
#define foreach_simple(cursor, listp) for(struct list *cursor = (listp)->next; cursor != (listp); cursor = cursor->next)
#define foreach(cursor, listp)                                  \
    for (struct list *cursor = (listp)->next, *n = cursor->next;                      \
         cursor != (listp);                                             \
         cursor = n, n = cursor->next)
void list_insert(struct list *list, struct list *item);
void list_remove(struct list *item);
void list_free_entry(struct list *list, void *entry);

//simple wrapper for a dynamic array represented as a (ptr, len, cap) triple
//capacity rule is cap *= 2
struct array {
    byte *data;
    int len;
    int cap;
    size_t item_size;
};
void array_init(struct array *a, size_t item_size);
void array_push(struct array *a, void *data);
void *array_pop(struct array *a);
void *array_get(struct array *a, int i);
void array_swap_remove(struct array *a, int *i);
void array_free(struct array *a);

//integer keys. you want string keys? hash ur string to an int first.
//factor of 0.75. linear probing.
struct hash {
    size_t item_size;
    int len;
    int cap;
    int *keys;
    byte *values;
    int *occupieds;
};
void hash_init(struct hash *h, size_t value_size);
void hash_insert(struct hash *h, int key, void *value);
void *hash_get(struct hash *h, int key);
void hash_remove(struct hash *h, int key);
void hash_free(struct hash *h);
//iter stuff
int *hash_next(struct hash *h, int *key);

#endif // __UTILS_H_
