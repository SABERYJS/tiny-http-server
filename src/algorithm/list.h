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
