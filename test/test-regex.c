//
// Created by Administrator on 2019/6/29.
//

#include <pcre.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include "regex.h"

int main(int argc, char *argv[]) {
    char *pattern = "^phone:(\\d{1,}):(\\w+):(\\d{2})$";
    char *source = "phone:13249808562:ShenZhen:12";
    int off;
    char *err;
    struct Regex *regex = RegexCreate(pattern, 3);
    struct RegexMatch *match = RegexExec(regex, source, 0);
    if (!match) {
        printf("match failed\n");
        exit(1);
    } else {
        printf("match successful\n");
        printf("substr:%s\n", RegexSubMatchAtIndex(match, 1));
        printf("substr:%s\n", RegexSubMatchAtIndex(match, 2));
        printf("substr:%s\n", RegexSubMatchAtIndex(match, 3));
    }
}
