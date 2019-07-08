//
// Created by Administrator on 2019/7/3 0003.
//

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
