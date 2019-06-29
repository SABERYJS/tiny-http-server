//
// Created by Administrator on 2019/6/18 0018.
//

#ifndef STL_CLONE_STACK_H
#define STL_CLONE_STACK_H

#include "config_header.h"
#include "list.h"

struct Stack {
    size_t count;
    struct List *list;
};


#define StackIsEmpty(s) (s->count==0)
#define STackSize(s) (s->count)

struct Stack *StackCreate();

static int StackNodeCompareMethod(struct ListNode *node, void *data) {}

static void StackNodeClearMethod(struct ListNode *node) {}

int StackPush(struct Stack *s, void *data);

void *StackTop(struct Stack *s);

void *StackNext(struct Stack *s);

void StackPop(struct Stack *s);


#endif //STL_CLONE_STACK_H
