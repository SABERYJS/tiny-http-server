//
// Created by Administrator on 2019/7/19 0019.
//

#include "base_64.h"


/**
 * normal char map to encoded char
 * **/
static char Base64CharMap(short c) {
    if (c >= 0 && c <= 25) {
        return 'A' + c;
    } else if (c >= 26 && c <= 51) {
        return 'a' + (c - 26);
    } else if (c >= '0' && c <= '9') {
        return '0' + (c - 52);
    } else if (c == 62) {
        return '+';
    } else {
        return '/';
    }
}

static short Base64CharDecode(char c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A';
    } else if (c >= 'a' && c <= 'z') {
        return c - 'a' + 26;
    } else if (c >= '0' && c <= '9') {
        return c - '0' + 52;
    } else if (c == '+') {
        return 62;
    } else {
        return 63;
    }
}

char *Base64_encrypt(char *src, size_t len) {
    unsigned int m = 0, n = 0, k = 0, a = 0;//32 bits
    int cycle = len / 3;
    int left = len % 3;
    int equalCount = (left == 0 ? 0 : (left == 1 ? 2 : 1));
    int memLen = cycle * 4 + (left == 0 ? 0 : (left == 1 ? 2 : 3)) + 1 + equalCount;//tail char '\0'
    char *buf;
    int i, j = 0;
    if (!(buf = MemAlloc(memLen))) {
        return NULL;
    } else {
        for (i = 0; i < cycle; i++) {
            m = n = k = 0;
            m = ((m | src[3 * i + 0]) << 16);
            n = ((n | src[3 * i + 1]) << 8);
            k = ((k | src[3 * i + 2]) << 0);
            a = m | n | k;
            buf[j++] = Base64CharMap((a & MASK_FOURTH_CHAR) >> 18);
            buf[j++] = Base64CharMap((a & MASK_THIRD_CHAR) >> 12);
            buf[j++] = (Base64CharMap((a & MASK_SECOND_CHAR) >> 6));
            buf[j++] = Base64CharMap((a & MASK_FIRST_CHAR) >> 0);
        }

        if (left > 0) {
            if (left == 1) {
                //need extra 4 bits (0)
                unsigned short s = 0;
                s = src[len - 1] << 4;
                buf[j++] = Base64CharMap(s >> 6);
                buf[j++] = Base64CharMap(s & MASK_FIRST_CHAR);
            } else {
                //left=2,so filling 2 bits
                unsigned int s = 0;
                m = src[len - 2] << 8;
                n = src[len - 1] << 0;
                s = (m | n) << 2;
                buf[j++] = Base64CharMap((s & MASK_THIRD_CHAR) >> 12);
                buf[j++] = Base64CharMap((s & MASK_SECOND_CHAR) >> 6);
                buf[j++] = Base64CharMap((s & MASK_FIRST_CHAR) >> 0);
            }
            buf[j++] = '=';
            if (equalCount == 2) {
                buf[j++] = '=';
            }
        }
        return buf;
    }
}


char *Base64_decrypt(char *src, size_t len) {
    unsigned int m = 0, n = 0, k = 0, h = 0, a = 0;//32 bits
    int cycle = len / 4;
    int left = len % 4;
    int equalCount = (left == 0 ? 0 : (left == 2 ? 2 : 1));
    len -= equalCount;
    int memLen = cycle * 3 + (left == 0 ? 0 : (left == 2 ? 1 : 2)) + 1;//tail char '\0'
    char *buf;
    int i, j = 0;
    if (!(buf = MemAlloc(memLen))) {
        return NULL;
    } else {
        for (i = 0; i < cycle; i++) {
            m = n = k = h = 0;
            m = ((m | Base64CharDecode(src[4 * i + 0])) << 18);
            n = ((n | Base64CharDecode(src[4 * i + 1])) << 12);
            k = ((k | Base64CharDecode(src[4 * i + 2])) << 6);
            h = ((h | Base64CharDecode(src[4 * i + 3])) << 0);
            a = m | n | k | h;
            buf[j++] = (a & BASE64_DECODE_MASK_THIRD) >> 16;
            buf[j++] = (a & BASE64_DECODE_MASK_SECOND) >> 8;
            buf[j++] = (a & BASE64_DECODE_MASK_FIRST) >> 0;
        }

        if (left > 0) {
            if (left == 2) {
                unsigned char s;
                s = ((Base64CharDecode(src[len - 2]) << 6) | (Base64CharDecode(src[len - 1]) << 0)) >> 4;
                buf[j++] = s;
            } else {
                unsigned int s;
                s = ((Base64CharDecode(src[len - 3]) << 12) | (Base64CharDecode(src[len - 2]) << 6) |
                     (Base64CharDecode(src[len - 1]) << 0)) >> 2;
                buf[j++] = s >> 8;
                buf[j++] = s & BASE64_DECODE_MASK_FIRST;
            }
        }
        return buf;
    }
}