/*Copyright (c) 2019 , saberyjs@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


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


