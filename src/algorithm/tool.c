//
// Created by Administrator on 2019/6/17.
//

#include "tool.h"

char *RandomChars(size_t len, int offset) {
    if (len <= 0) {
        return NULL;
    }
    char *s = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t ls = strlen(s);
    char *r = (char *) malloc(sizeof(char) * len + 1);
    if (!r) {
        return NULL;
    }
    size_t i = 0;
    srand(time(NULL) + offset);
    while (len-- > 0) {
        *(r + i++) = s[rand() % ls];
    }
    *(r + len) = 0;
    return r;
}
