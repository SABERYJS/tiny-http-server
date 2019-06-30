/**
 * reference http://www.pcre.org/original/doc/html/
 * reference https://blog.csdn.net/shenwansangz/article/details/49763727
 * **/

#ifndef QC_PCRE_H
#define QC_PCRE_H

#include <pcre.h>
#include <string.h>
#include <stdio.h>

struct Regex {
    char *pattern;
    struct pcre *pcre;
    int subCount;
};

struct RegexMatch {
    char *source;
    int len;
    int *vector;
};


struct Regex *RegexCreate(char *pattern, int subCount);

void RegexFree(struct Regex *regex);

struct RegexMatch *RegexExec(struct Regex *regex, char *source, int pos);


char *RegexSubMatchAtIndex(struct RegexMatch *match, int index);


void FreeMatch(struct RegexMatch *match);

#endif //QC_PCRE_H
