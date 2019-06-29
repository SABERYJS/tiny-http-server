//
// Created by Administrator on 2019/6/17 0017.
//

#ifndef STL_CLONE_ARRAY_H
#define STL_CLONE_ARRAY_H

#include "array.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

struct Array {
    int capacity;//array capacity
    size_t size; //size of every  unit
    int count; //storage unit count
    void *data;// start address pointer
    size_t t_items;//max items to storage
};

#define ArrayEmpty(a) (a.count==0)

static void *ReMalloc(size_t new_size);

struct Array *ArrayCreate(size_t capacity, size_t size);

void *ArrayNext(struct Array *a, size_t nblock);

void ArrayDestroy(struct Array *a);


#endif //STL_CLONE_ARRAY_H
