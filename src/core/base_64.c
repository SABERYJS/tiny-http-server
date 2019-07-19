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

#include "base_64.h"


/**
 * normal char map to encoded char
 * **/
static char Base64CharMap(short c) {
    if (c >= 0 && c <= 25) {
        return 'A' + c;
    } else if (c >= 26 && c <= 51) {
        return 'a' + (c - 26);
    } else if (c >= 52 && c <= 61) {
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

char *Base64Encrypt(char *src, size_t len) {
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


char *Base64Decrypt(char *src, size_t len) {
    unsigned int m = 0, n = 0, k = 0, h = 0, a = 0;//32 bits
    int equalCount = 0;
    if (src[len - 1] == '=') {
        equalCount++;
    }
    if (src[len - 2] == '=') {
        equalCount++;
    }

    len -= equalCount;

    int cycle = len / 4;
    int left = len % 4;

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
                buf[j] = s;
            } else {
                unsigned int s;
                s = ((Base64CharDecode(src[len - 3]) << 12) | (Base64CharDecode(src[len - 2]) << 6) |
                     (Base64CharDecode(src[len - 1]) << 0)) >> 2;
                buf[j++] = s >> 8;
                buf[j] = s & BASE64_DECODE_MASK_FIRST;
            }
        }
        return buf;
    }
}