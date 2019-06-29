//
// Created by Administrator on 2019/6/16.
//

#ifndef C___LIST_H
#define C___LIST_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct ListNode {
    void *data;
    struct ListNode *next;
    struct ListNode *prev;
};

struct List {
    struct ListNode *header;
    struct ListNode *tail;

    int (*compareCallback)(struct ListNode *node, void *data);

    void (*clearCallback)(struct ListNode *node);

    int count;
};


struct List *createList(int (*compare)(struct ListNode *, void *), void (*clear)(struct ListNode *));

int appendNode(struct List *list, void *data);

void iterateList(struct List *list, void(*callback)(struct ListNode *node));

static void handleRemove(struct List *list, struct ListNode *tNode);

void removeNode(struct List *list, struct ListNode *node);


void removeNodeByData(struct List *list, void *data);

struct ListNode *searchNode(struct List *list, void *data);

void clearList(struct List *list);


#define isEmpty(list) (list->count==0)

#define nodeData(node) (node->data)

#endif //C___LIST_H
