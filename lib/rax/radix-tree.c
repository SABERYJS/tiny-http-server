
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

