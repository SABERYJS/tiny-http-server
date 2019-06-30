//
// Created by Administrator on 2019/6/30.
//

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