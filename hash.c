//
// Created by Administrator on 2019/6/17.
// reference document https://blog.csdn.net/hustyangju/article/details/47012139
//

#include "hash.h"
#include "unit.h"

/**
 * bkdr has algorithm

 * **/
unsigned int BkdrHashMap(char *key) {
    unsigned int seed = 131;
    unsigned int hash = 0;

    while (*key) {
        hash = hash * seed + (*key++);
    }
    return (hash & 0x7FFFFFFF);
}

struct HashTable *HashTableCreate(size_t bucket_size, unsigned int (*hashMap)(char *)) {
    if (!hashMap) {
        hashMap = BkdrHashMap;
    }
    struct HashTable *table = malloc(sizeof(struct HashTable));
    if (!table) {
        return NULL;
    } else {
        table->count = 0;
        table->bucket_size = bucket_size;
        table->hashMap = hashMap;
        table->data = malloc(sizeof(struct HashTableUnit) * bucket_size);
        if (!table->data) {
            free(table);
            return NULL;
        } else {
            return table;
        }
    }
}


int HashCompareMethod(struct ListNode *node, void *data) {
    struct HashUnit *u = (struct HashUnit *) node->data;
    char *k = (char *) data;
    //printf("node:%s,value:%d - %s\n", u->key, ((struct Unit *) u->data)->a, k);
    return strcmp(u->key, k);
}

void HashClear(struct ListNode *node) {}

int HashAdd(struct HashTable *hashTable, char *key, void *data) {
    unsigned int t = BkdrHashMap(key);
    unsigned int idx = t % hashTable->bucket_size;
    struct HashTableUnit *tUnit = hashTable->data + idx;
    if (!tUnit->data) {
        tUnit->data = createList(HashCompareMethod, HashClear);
        if (!tUnit->data) {
            return -1;
        }
    }
    struct HashUnit *hu = malloc(sizeof(struct HashUnit));
    if (!hu) {
        return -1;
    } else {
        hu->key = key;
        hu->data = data;
        appendNode(tUnit->data, hu);
        return 0;
    }
}

void HashDelete(struct HashTable *table, char *key) {
    unsigned int idx = BkdrHashMap(key) % table->bucket_size;
    struct HashTableUnit *tUnit = table->data + idx;
    if (!tUnit->data) {
        return;
    } else {
        struct List *lst = tUnit->data;
        removeNodeByData(lst, key);
    }
}

void *HashSearch(struct HashTable *table, char *key) {
    unsigned int idx = BkdrHashMap(key) % table->bucket_size;
    struct HashTableUnit *tUnit = table->data + idx;
    if (!tUnit->data) {
        return NULL;
    } else {
        struct ListNode *node = searchNode(tUnit->data, key);
        if (!node) {
            return NULL;
        } else {
            return ((struct HashUnit *) node->data)->data;
        }
    }
}


