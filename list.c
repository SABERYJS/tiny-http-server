//
// Created by Administrator on 2019/6/16.
//

#include "list.h"

struct List *createList(int (*compare)(struct ListNode *, void *), void (*clear)(struct ListNode *)) {
    struct List *list = (struct List *) malloc(sizeof(struct List));
    if (!list) {
        return NULL;
    } else {
        list->header = list->tail = NULL;
        list->count = 0;
        list->compareCallback = compare;
        list->clearCallback = clear;
        return list;
    }
}

int appendNode(struct List *list, void *data) {
    struct ListNode *node = (struct ListNode *) malloc(sizeof(struct ListNode));
    if (!node) {
        return -1;
    } else {
        node->data = data;
        node->next = NULL;
        node->prev = NULL;
        if (list->count == 0) {
            list->header = list->tail = node;
        } else {
            node->prev = list->tail;
            list->tail->next = node;
            list->tail = node;
        }
        ++list->count;
    }
    return 0;
}

void iterateList(struct List *list, void (*callback)(struct ListNode *)) {
    struct ListNode *tNode = list->header;
    while (tNode) {
        callback(tNode);
        tNode = tNode->next;
    }
}

void handleRemove(struct List *list, struct ListNode *tNode) {
    if (list->count == 1) {
        list->header = list->tail = NULL;
        list->count--;
    } else if (tNode == list->header) {
        list->header = list->header->next;
        list->header->prev = NULL;
        list->count--;
    } else {
        if (list->tail == tNode) {
            list->tail->prev->next = NULL;
            list->tail = list->tail->prev;
            list->count--;
        } else {
            tNode->prev->next = tNode->next;
            tNode->next->prev = tNode->prev;
            list->count--;
        }
    }
}

void removeNode(struct List *list, struct ListNode *node) {
    if (list->count == 0) {
        return;
    }
    struct ListNode *tNode = list->header;
    while (tNode) {
        if (node == tNode) {
            handleRemove(list, tNode);
            return;
        }
        tNode = tNode->next;
    }
}

void removeNodeByData(struct List *list, void *data) {
    if (list->count == 0) {
        return;
    }
    struct ListNode *tNode = list->header;
    while (tNode) {
        if (list->compareCallback(tNode, data) == 0) {
            handleRemove(list, tNode);
        }
        tNode = tNode->next;
    }
}

struct ListNode *searchNode(struct List *list, void *data) {
    if (list->count == 0) {
        return NULL;
    }
    struct ListNode *tNode = list->header;
    while (tNode) {
        if (list->compareCallback(tNode, data) == 0) {
            return tNode;
        }
        tNode = tNode->next;
    }
    return NULL;
}

void clearList(struct List *list) {
    iterateList(list, list->clearCallback);
    list->tail = list->header = NULL;
    list->count = 0;
}

