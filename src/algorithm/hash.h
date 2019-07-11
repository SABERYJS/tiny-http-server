//
// Created by Administrator on 2019/6/17.
//

#ifndef STL_CLONE_HASH_H
#define STL_CLONE_HASH_H

#include <unistd.h>
#include "list.h"
#include <string.h>

struct HashUnit {
    char *key;
    void *data;
};

struct HashTableUnit {
    struct List *data;
};


struct HashTable {
    unsigned int (*hashMap)(char *);

    size_t bucket_size;
    size_t count;
    struct HashTableUnit *data;
};


unsigned int BkdrHashMap(char *key);

int HashCompareMethod(struct ListNode *node, void *data);

struct HashTable *
HashTableCreate(size_t bucket_size, unsigned int (*hashMap)(char *));

int HashAdd(struct HashTable *hashTable, char *key, void *data);

void HashDelete(struct HashTable *table, char *key);

void *HashSearch(struct HashTable *table, char *key);


typedef int (*HashIterateCallback)(char *key, void *data);

void *HashIterate(struct HashTable *table, HashIterateCallback callback);

#endif //STL_CLONE_HASH_H
