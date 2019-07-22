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

#include "event.h"

struct EventDepositary *EventCreateDepositary(struct timeval *out) {
    struct EventDepositary *depositary = (struct EventDepositary *) malloc(sizeof(struct EventDepositary));
    if (!depositary) {
        return NULL;
    } else {
        depositary->rbTree = RbTreeCreate(compare, NULL, NULL);
        depositary->pending_add = createList(NULL, NULL);
        depositary->pending_deleted = createList(NULL, NULL);
        if (!depositary->rbTree || !depositary->pending_add || !depositary->pending_deleted) {
            if (depositary->rbTree) {
                free(depositary->rbTree);
            }
            if (depositary->pending_deleted) {
                free(depositary->pending_deleted);
            }
            if (depositary->pending_add) {
                free(depositary->pending_add);
            }
            free(depositary);
            return NULL;
        } else {
            depositary->tv = out;
            depositary->status = DEPOSITARY_IDLE;
            FD_ZERO(&depositary->rs);
            FD_ZERO(&depositary->ws);
            FD_ZERO(&depositary->es);
            return depositary;
        }
    }
}

int EventAdd(struct EventDepositary *depositary, unsigned int type, int fd, void *data, eventHandleCallback callback) {
    if (type != EVENT_READABLE && type != EVENT_WRITEABLE && type != EVENT_ERROR) {
        return -1;
    } else {
        //depositary is busy,so insert later
        if (depositary->status == DEPOSITARY_BUSY) {
            struct EventPendingAdd *pending = MemAlloc(sizeof(struct EventPendingAdd));
            if (!pending) {
                return -1;
            }
            pending->fd = fd;
            pending->flag = type;
            pending->data = data;
            pending->callback = callback;
            if (appendNode(depositary->pending_add, pending) < 0) {
                free(pending);
                return -1;
            }
            return 1;
        }
        struct EventHandler *handler = (struct EventHandler *) malloc(sizeof(struct EventHandler));
        if (!handler) {
            return -1;
        } else {
            handler->fd = fd;
            struct RbTreeNode *node = RbTreeSearch(depositary->rbTree, handler);
            int set = 0;
            if (!node) {
                handler->data = data;
                handler->callback = callback;
                handler->ref = 0;
                handler->flag = 0;
                handler->depositary = depositary;
                if (!RbTreeInsertNode(depositary->rbTree, handler)) {
                    return -1;
                }
            } else {
                free(handler);
                handler = (struct EventHandler *) node->data;
                if (data) {
                    handler->data = data;
                }
                if (callback) {
                    handler->callback = callback;
                }
            }
            if (type == EVENT_READABLE && !FD_ISSET(fd, &depositary->rs)) {
                FD_SET(fd, &depositary->rs);
                set = 1;
            } else if (type == EVENT_WRITEABLE && !FD_ISSET(fd, &depositary->ws)) {
                FD_SET(fd, &depositary->ws);
                set = 1;
            } else {
                if (!FD_ISSET(fd, &depositary->es)) {
                    FD_SET(fd, &depositary->es);
                    set = 1;
                }
            }
            if (set) {
                ++handler->ref;
                handler->flag |= type;
            }
            return 1;
        }
    }
}

int EventRemove(struct EventDepositary *depositary, unsigned int type, int fd) {
    if (type != EVENT_READABLE && type != EVENT_WRITEABLE && type != EVENT_ERROR) {
        return -1;
    } else {
        if (depositary->status == DEPOSITARY_BUSY) {
            struct EventPendingDelete *pendingDelete = MemAlloc(sizeof(struct EventPendingDelete));
            if (!pendingDelete) {
                return -1;
            }
            pendingDelete->fd = fd;
            pendingDelete->flag = type;
            if (appendNode(depositary->pending_deleted, pendingDelete) < 0) {
                free(pendingDelete);
                return -1;
            }
            return 1;
        }
        struct EventHandler *handler = (struct EventHandler *) malloc(sizeof(struct EventHandler));
        if (!handler) {
            return -1;
        } else {
            handler->fd = fd;
            struct RbTreeNode *node = RbTreeSearch(depositary->rbTree, handler);
            free(handler);
            if (node) {
                handler = (struct EventHandler *) node->data;
                if (type == EVENT_READABLE && FD_ISSET(fd, &depositary->rs)) {
                    FD_CLR(fd, &depositary->rs);
                } else if (type == EVENT_WRITEABLE && FD_ISSET(fd, &depositary->ws)) {
                    FD_CLR(fd, &depositary->ws);
                } else if (FD_ISSET(fd, &depositary->es)) {
                    FD_CLR(fd, &depositary->es);
                }
                --handler->ref;
                handler->flag &= (~type);
                if (!handler->ref) {
                    RbTreeDeleteNode(depositary->rbTree, node);
                }
            }
            return 1;
        }
    }
}

static void EventReInitSingleFd(struct RbTreeNode *node) {
    struct EventHandler *handler = (struct EventHandler *) node->data;
    struct EventDepositary *depositary = handler->depositary;
    if (handler->flag & EVENT_READABLE) {
        printf("socket[%d] add readable event\n", handler->fd);
        FD_SET(handler->fd, &depositary->rs);
    }
    if (handler->flag & EVENT_WRITEABLE) {
        printf("socket[%d] add writeable event\n", handler->fd);
        FD_SET(handler->fd, &depositary->ws);
    }
    if (handler->flag & EVENT_ERROR) {
        FD_SET(handler->fd, &depositary->es);
    }
}

static void EventReInitLoop(struct EventDepositary *depositary) {
    FD_ZERO(&depositary->rs);
    FD_ZERO(&depositary->ws);
    FD_ZERO(&depositary->es);
    RbTreeIterate(depositary->rbTree->root, EventReInitSingleFd);
}

static void EventHandleCallback(struct RbTreeNode *node) {
    struct EventHandler *handler = (struct EventHandler *) node->data;
    printf("EventHandleCallback entered,fd is:%d\n", handler->fd);
    struct EventDepositary *depositary = handler->depositary;
    if (FD_ISSET(handler->fd, &depositary->rs)) {
        handler->callback(EVENT_READABLE, handler->data);
    }
    if (FD_ISSET(handler->fd, &depositary->ws)) {
        handler->callback(EVENT_WRITEABLE, handler->data);
    }
    if (FD_ISSET(handler->fd, &depositary->es)) {
        handler->callback(EVENT_ERROR, handler->data);
    }
}

int EventLoop(struct EventDepositary *depositary) {
    printf("enter  event loop again\n");
    int ret;
    EventReInitLoop(depositary);
    ret = select(FD_SETSIZE, &depositary->rs, &depositary->ws, &depositary->es, depositary->tv);
    if (ret < 0) {
        return -1;
    } else {
        if (ret > 0) {
            depositary->status = DEPOSITARY_BUSY;
            RbTreeIterate(depositary->rbTree->root, EventHandleCallback);
            depositary->status = DEPOSITARY_IDLE;
            //after iteration finish,we start to handle pending add or delete request
            EventHandlePendingDelete(depositary);//delete fd
            EventHandlePendingAdd(depositary);//add fd
        }
        return 1;
    }
}

static void EventHandlePendingDelete(struct EventDepositary *depositary) {
    struct ListNode *lm;
    struct EventPendingDelete *delete;
    //remember that we remove  specified fd firstly
    while (depositary->pending_deleted->count > 0) {
        lm = depositary->pending_deleted->header;
        delete = (struct EventPendingDelete *) lm->data;
        EventRemove(depositary, delete->flag, delete->fd);
        removeNode(depositary->pending_deleted, lm);//remove node from list
        free(lm->data);// free node data
        free(lm);//free current node
    }
}

static void EventHandlePendingAdd(struct EventDepositary *depositary) {
    struct ListNode *ld;
    struct EventPendingAdd *add;
    while (depositary->pending_add->count > 0) {
        ld = depositary->pending_add->header;
        add = (struct EventPendingAdd *) ld->data;
        EventAdd(depositary, add->flag, add->fd, add->data, add->callback);
        removeNode(depositary->pending_add, ld);
        free(ld->data);
        free(ld);
    }
}