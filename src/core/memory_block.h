//
// Created by Administrator on 2019/7/18 0018.
//

#ifndef STL_CLONE_MEMORY_BLOCK_H
#define STL_CLONE_MEMORY_BLOCK_H

#include "global_header.h"

struct MemoryBlock {
    void *data;
    off_t next;
};
struct MemoryBlockAllocator {
    size_t size;//each block size
    size_t count;//count of blocks
    struct List *list; //list to link blocks
    struct MemoryBlock *prev_block;//memory  block that allocate memory  last time
};

struct MemoryBlockAllocator *MemoryBlockAllocatorCreate(size_t size);

void *MemoryBlockAllocate(struct MemoryBlockAllocator *allocator, size_t len);

#endif //STL_CLONE_MEMORY_BLOCK_H
