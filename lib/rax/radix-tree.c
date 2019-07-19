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
/**
 * about rax ,please reference https://github.com/antirez/rax
 * **/

#include "radix-tree.h"
#include <error.h>
#include <errno.h>

struct rax *RadixTreeCreate() {
    return raxNew();
}

/**
 * @return  return 1 if insert successful,return 0 if key exist and update successful,
 * and return -1 if no more memory
 * **/
int RadixTreeInsert(struct rax *rax, unsigned char *s, size_t len, void *data, void **old) {
    int ret = raxInsert(rax, s, len, data, old);
    if (ret > 0) {
        return 1;
    } else if (ret == 0) {
        if (errno == ENOMEM) {
            return -1;
        } else {
            return 0;
        }
    }
}

/**
 * @return return  search result if find it ,otherwise return NULL
 * **/
void *RadixTreeSearch(struct rax *rax, unsigned char *key, size_t len) {
    void *data = raxFind(rax, key, len);
    if (data == raxNotFound) {
        return NULL;
    } else {
        return data;
    }
}

/**
 * @return return 1 if delete successful ,otherwise return -1 if key can not be found
 * **/
int RadixTreeDelete(struct rax *rax, unsigned char *key, size_t len, void **old) {
    int ret = raxRemove(rax, key, len, old);
    return ret > 0 ? ret : -1;
}

