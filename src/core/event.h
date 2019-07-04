//
// Created by Administrator on 2019/7/3 0003.
//

#ifndef QC_EVENT_H
#define QC_EVENT_H

#include "global_header.h"

#define EVENT_READABLE 1
#define EVENT_WRITEABLE 2
#define EVENT_ERROR 4

typedef void (*eventHandleCallback)(int type, void *data);

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
    struct timeval *tv;
    struct RbTree *rbTree;
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

static void test(int n, struct fd_set *p) {
    printf("fdbits:%lu\n", NFDBITS);
    //printf("test bit:%lu\n", (p)->fds_bits[(n) / NFDBITS]);
    printf("test bit:%lu\n", (p)->fds_bits[1]);
    ((p)->fds_bits[(n) / NFDBITS] |= (1L << ((n) % NFDBITS)));
}

#endif //QC_EVENT_H
