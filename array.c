//
// Created by Administrator on 2019/6/17 0017.
//

#include "array.h"

struct Array *ArrayCreate(size_t capacity, size_t size) {
    struct Array *a = malloc(sizeof(struct Array));
    if (!a) {
        return NULL;
    } else {
        a->capacity = capacity;
        a->size = size;
        a->t_items = floor(capacity / size);
        a->count = 0;
        a->data = malloc(a->t_items * a->size);
        if (!a->data) {
            free(a);
            return NULL;
        } else {
            return a;
        }
    }
}

static void *ReMalloc(size_t new_size) {
    void *tp = malloc(new_size);
    if (!tp) {
        return NULL;
    } else {
        memset(tp, 0, new_size);
        return tp;
    }
}

void *ArrayNext(struct Array *a, size_t nblock) {
    if (a->count == a->t_items) {
        size_t nz = a->capacity * 2;
        void *np = ReMalloc(nz);
        if (!np) {
            return NULL;
        } else {
            memcpy(np, a->data, a->capacity);
            a->capacity = nz;
            a->t_items = floor(nz / a->size);
            free(a->data);
            a->data = np;
        }
    }
    void *data = a->data + a->count * a->size;
    a->count += nblock;
    return data;
}

void ArrayDestroy(struct Array *a) {
    free(a->data);
    free(a);
}