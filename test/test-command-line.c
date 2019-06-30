//
// Created by Administrator on 2019/6/30.
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../src/core/command_line.h"
#include "../src/algorithm/list.h"

void callback(struct ListNode *node) {
    struct CommandOption *option = (struct CommandOption *) node->data;
    printf("node name:%s,node value:%s\n", option->name, option->value ?: "");
}


int main(/*int argc, char *argv[]*/) {
    int argc = 5;
    char *argv[] = {"./a.exe", "--a", "-b", "1", "--c"};
    struct CommandLineArgs *args = ParseCommandLine(argc, argv);
    if (!args) {
        printf("parse command line failed\n");
        exit(1);
    } else {
        printf("parse command line successful\n");
    }
    printf("command line");

    iterateList(args->options, callback);
}