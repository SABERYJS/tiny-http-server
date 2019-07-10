//
// Created by Administrator on 2019/7/10 0010.
//

#include "string-tool.h"

char *trim(const char *source, size_t len, char c) {
    const char *ps = source;
    const char *pe = source + len - 1;
    int i = 0, j = 0, l;
    char *buf;
    while (*ps++ == c)i++;
    while (*pe == c || *pe == '\0') {
        //tail has many '\0'
        j++;
        pe--;
    }
    l = len - j - i;//left length
    buf = MemAlloc(l + 1);
    if (!buf) {
        return NULL;
    } else {
        memcpy(buf, source + i, l);
        buf[l] = 0;
        return buf;
    }
}