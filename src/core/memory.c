//
// Created by Administrator on 2019/7/5 0005.
//
#include "memory.h"

void *MemAlloc(size_t l) {
    void *p = malloc(l);
    if (!p) {
        return NULL;
    } else {
        memset(p, 0, l);
        return p;
    }
}

void *MemFree(void *p) {
    free(p);
}
