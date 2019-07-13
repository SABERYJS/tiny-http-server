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


#ifndef QC_EVENT_H
#define QC_EVENT_H

#include "global_header.h"

#define EVENT_READABLE 1
#define EVENT_WRITEABLE 2
#define EVENT_ERROR 4

#define DEPOSITARY_IDLE 1
#define DEPOSITARY_BUSY 2

typedef void (*eventHandleCallback)(int type, void *data);

struct EventPendingAdd {
    unsigned int fd;
    void *data;
    unsigned int flag;
    eventHandleCallback callback;
};

struct EventPendingDelete {
    unsigned int fd;
    unsigned int flag;
};

struct EventHandler {
    unsigned int fd;
    void *data;
    int ref;
    unsigned int flag;
    eventHandleCallback callback;
    struct EventDepositary *depositary;
};

struct EventDepositary {
    fd_set rs;
    fd_set ws;
    fd_set es;
    short status;
    struct timeval *tv;
    struct RbTree *rbTree;
    struct List *pending_add;//event to add
    struct List *pending_deleted;//event to deleted
};

static int compare(struct RbTreeNode *a, struct RbTreeNode *b) {
    struct EventHandler *ha = (struct EventHandler *) a->data;
    struct EventHandler *hb = (struct EventHandler *) b->data;
    return ha->fd > hb->fd ? 1 : (ha->fd == hb->fd ? 0 : -1);
}


struct EventDepositary *EventCreateDepositary(struct timeval *out);

int EventAdd(struct EventDepositary *depositary, unsigned int type, int fd, void *data, eventHandleCallback callback);

int EventRemove(struct EventDepositary *depositary, unsigned int type, int fd);

int EventLoop(struct EventDepositary *depositary);

static void EventReInitSingleFd(struct RbTreeNode *node);

static void EventHandleEvent(struct RbTreeNode *node);

static void EventReInitLoop(struct EventDepositary *depositary);

static void EventHandlePendingAdd(struct EventDepositary *depositary);

static void EventHandlePendingDelete(struct EventDepositary *depositary);

#endif //QC_EVENT_H
