//
// Created by Administrator on 2019/6/29.
//

#ifndef QC_RADIX_TREE_H
#define QC_RADIX_TREE_H

#include "rax.h"
#include <stdlib.h>

struct rax *RadixTreeCreate();

int RadixTreeInsert(struct rax *rax, unsigned char *s, size_t len, void *data, void **old);

void *RadixTreeSearch(struct rax *rax, unsigned char *key, size_t len);

int RadixTreeDelete(struct rax *rax, unsigned char *key, size_t len, void **old);

#endif //QC_RADIX_TREE_H
