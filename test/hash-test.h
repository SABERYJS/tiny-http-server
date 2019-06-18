//
// Created by Administrator on 2019/6/17.
//

#ifndef  TEST_HASH
#define TEST_HASH

#include "../hash.h"
#include "../tool.h"
#include "../unit.h"

void testHash() {
    struct HashTable *table = HashTableCreate(100000, NULL);
    char *tk;
    for (int i = 0; i < 100000; i++) {
        struct Unit *u = malloc(sizeof(struct Unit));
        u->a = i;
        char *key = RandomChars(32, i);
        if (i == 1000) {
            tk = key;
        }
        HashAdd(table, key, u);
    }

    struct Unit *tpu = (struct Unit *) HashSearch(table, tk);
    if (!tpu) {
        printf("no result\n");
    } else {
        printf("search result:%d\n", tpu->a);
    }

    HashDelete(table, tk);
    tpu = (struct Unit *) HashSearch(table, tk);
    if (!tpu) {
        printf("no result\n");
    } else {
        printf("search result:%d\n", tpu->a);
    }
    tpu = (struct Unit *) HashSearch(table, tk);
    if (!tpu) {
        printf("no result\n");
    } else {
        printf("search result:%d\n", tpu->a);
    }

    tpu = (struct Unit *) HashSearch(table, tk);
    if (!tpu) {
        printf("no result\n");
    } else {
        printf("search result:%d\n", tpu->a);
    }
}

#endif