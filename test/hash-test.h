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

#ifndef  TEST_HASH
#define TEST_HASH

#include "../src/algorithm/hash.h"
#include "../src/algorithm/tool.h"
#include "../src/algorithm/unit.h"

void testHash() {
    struct HashTable *table = HashTableCreate(100000, NULL);
    char *tk;
    int i;
    for (i = 0; i < 100000; i++) {
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