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
    handleRemove(list, node);
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

