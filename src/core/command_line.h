//
// Created by Administrator on 2019/6/30.
//

#ifndef QC_COMMAND_LINE_H
#define QC_COMMAND_LINE_H

#include "../algorithm/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define  COMMAND_LOG_OPEN

struct CommandOption {
    char *name;
    char *value;
};

struct CommandLineArgs {
    struct List *options;
};

struct CommandLineArgs *ParseCommandLine(int argc, char *argv[]);

static int CommandOptionCompare(struct ListNode *node, void *data);

static void CommandOptionClear(struct ListNode *node);

#endif //QC_COMMAND_LINE_H
