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


#include "array.h"

struct Array *ArrayCreate(size_t capacity, size_t size) {
    struct Array *a = malloc(sizeof(struct Array));
    if (!a) {
        return NULL;
    } else {
        a->capacity = capacity;
        a->size = size;
        a->t_items = floor(capacity / size);
        a->count = 0;
        a->data = malloc(a->t_items * a->size);
        if (!a->data) {
            free(a);
            return NULL;
        } else {
            return a;
        }
    }
}

static void *ReMalloc(size_t new_size) {
    void *tp = malloc(new_size);
    if (!tp) {
        return NULL;
    } else {
        memset(tp, 0, new_size);
        return tp;
    }
}

void *ArrayNext(struct Array *a, size_t nblock) {
    if (a->count == a->t_items) {
        size_t nz = a->capacity * 2;
        void *np = ReMalloc(nz);
        if (!np) {
            return NULL;
        } else {
            memcpy(np, a->data, a->capacity);
            a->capacity = nz;
            a->t_items = floor(nz / a->size);
            free(a->data);
            a->data = np;
        }
    }
    void *data = a->data + a->count * a->size;
    a->count += nblock;
    return data;
}

void ArrayDestroy(struct Array *a) {
    free(a->data);
    free(a);
}