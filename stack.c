//
// Created by Administrator on 2019/6/18 0018.
//

#include "stack.h"

struct Stack *StackCreate() {
    struct Stack *s = malloc(sizeof(struct Stack));
    if (!s) {
        return NULL;
    } else {
        s->count = 0;
        s->list = createList(StackNodeCompareMethod, StackNodeClearMethod);
        if (!s->list) {
            free(s);
            return NULL;
        } else {
            return s;
        }
    }
}

int StackPush(struct Stack *s, void *data) {
    int ret = appendNode(s->list, data);
    if (ret != -1) {
        s->count++;
    }
    return ret;
}

void *StackTop(struct Stack *s) {
    if (StackIsEmpty(s)) {
        return NULL;
    } else {
        return s->list->tail->data;
    }
}

void StackPop(struct Stack *s) {
    if (StackIsEmpty(s)) {
        return;
    } else {
        removeNode(s->list, s->list->tail);
        s->count--;
    }
}

void *StackNext(struct Stack *s) {
    if (StackIsEmpty(s)) {
        return NULL;
    } else {
        void *data = StackTop(s);
        StackPop(s);
        return data;
    }
}