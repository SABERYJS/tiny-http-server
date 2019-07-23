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

char *strlwr(char *str) {
    unsigned char *p = (unsigned char *) str;

    while (*p) {
        *p = tolower((unsigned char) *p);
        p++;
    }

    return str;
}

char *strReplace(char *source, char t, char r) {
    int i = 0;
    size_t len = strlen(source);
    while (i < len) {
        if (source[i] == t) {
            source[i] = r;
        }
        i++;
    }
    return source;
}

char *strFirstToUpper(char *source) {
    if ((source[0] >= 'a' && source[0] <= 'z')) {
        source[0] = source[0] - 32;
    }
    return source;
}

char *strFirstToLower(char *source) {
    if ((source[0] >= 'A' && source[0] <= 'Z')) {
        source[0] = source[0] + 32;
    }
    return source;
}