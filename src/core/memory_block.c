//
// Created by Administrator on 2019/7/18 0018.
//

#include "memory_block.h"

struct MemoryBlockAllocator *MemoryBlockAllocatorCreate(size_t size) {
    struct MemoryBlockAllocator *allocator = MemAlloc(sizeof(struct MemoryBlockAllocator));
    if (!allocator) {
        return NULL;
    } else {
        allocator->list = createList(NULL, NULL);
        if (!allocator->list) {
            MemFree(allocator);
            return NULL;
        } else {
            allocator->size = size;
            allocator->count = 0;
            allocator->prev_block = NULL;
            return allocator;
        }
    }
}

void *MemoryBlockAllocate(struct MemoryBlockAllocator *allocator, size_t len) {
    struct MemoryBlock *block;
    if (!allocator->count || (allocator->size - allocator->prev_block->next) < len) {
        block = MemAlloc(sizeof(struct MemoryBlock));
        if (!block) {
            return NULL;
        } else {
            if (appendNode(allocator->list, block) < 0) {
                MemFree(block);
                return NULL;
            }
            block->data = MemAlloc(sizeof(allocator->size));
            if (!block->data) {
                MemFree(block);
                return NULL;
            } else {
                block->next += len;
            }
            allocator->prev_block = block;
            allocator->count++;
            return block->data;
        }
    } else {
        void *data = allocator->prev_block->data + allocator->prev_block->next;
        allocator->prev_block->next += len;
        return data;
    }
}
