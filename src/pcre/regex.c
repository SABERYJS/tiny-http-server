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


#include "regex.h"

struct Regex *RegexCreate(char *pattern, int subCount) {
    char *err;
    int off;
    struct pcre *pcre = (struct pcre *) pcre_compile(pattern, 0, (const char **) &err, &off, NULL);
    if (!pcre) {
        free(err);//allocated by pcre library
        return NULL;
    } else {
        struct Regex *regex = malloc(sizeof(struct Regex));
        if (!regex) {
            free(pcre);
            return NULL;
        } else {
            regex->pattern = pattern;
            regex->pcre = pcre;
            regex->subCount = subCount;
            return regex;
        }
    }
}


struct RegexMatch *RegexExec(struct Regex *regex, char *source, int pos) {
    size_t size = (1 + regex->subCount) * 3 * 2;
    int *vector = malloc(sizeof(int) * size);
    if (!vector) {
        return NULL;
    } else {
        size_t len = strlen(source);
        int ret = pcre_exec((const pcre *) regex->pcre, NULL, source, len, pos, 0, vector, size);
        if (ret < 0) {
            free(vector);
            return NULL;
        } else {
            struct RegexMatch *match = malloc(sizeof(struct RegexMatch));
            if (!match) {
                free(vector);
                return NULL;
            } else {
                match->vector = vector;
                match->source = source;
                match->len = len;
                return match;
            }
        }
    }
}

void RegexFree(struct Regex *regex) {
    free(regex->pcre);
}


char *RegexSubMatchAtIndex(struct RegexMatch *match, int index) {
    int end = match->vector[2 * index + 1];
    int start = match->vector[2 * index];
    int len = end - start + 1;
    char *str = malloc(sizeof(char) * len);
    if (!str) {
        return NULL;
    }
    memcpy(str, match->source + start, len - 1);
    str[len - 1] = 0;
    return str;
}

void FreeMatch(struct RegexMatch *match) {
    free(match->vector);
    free(match->source);
}