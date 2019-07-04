//
// Created by Administrator on 2019/7/3 0003.
//

#include "event.h"

struct EventDepositary *EventCreateDepositary(struct timeval *out) {
    struct EventDepositary *depositary = (struct EventDepositary *) malloc(sizeof(struct EventDepositary));
    if (!depositary) {
        return NULL;
    } else {
        depositary->rbTree = RbTreeCreate(compare, NULL, NULL);
        if (!depositary->rbTree) {
            free(depositary);
            return NULL;
        } else {
            depositary->tv = out;
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
        struct EventHandler *handler = (struct EventHandler *) malloc(sizeof(struct EventHandler));
        if (!handler) {
            return -1;
        } else {
            handler->fd = fd;
            struct RbTreeNode *node = RbTreeSearch(depositary->rbTree, handler);
            int set = 0;
            free(handler);
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
    printf("EventReInitSingleFd start\n");
    if (handler->flag | EVENT_READABLE) {
        printf("here\n");
        FD_SET(handler->fd, &depositary->rs);
        printf("readable\n");
    }
    if (handler->flag | EVENT_WRITEABLE) {
        FD_SET(handler->fd, &depositary->ws);
    }
    if (handler->flag | EVENT_ERROR) {
        FD_SET(handler->fd, &depositary->es);
    }
    printf("EventReInitSingleFd finished\n");
}

static void EventReInitLoop(struct EventDepositary *depositary) {
    FD_ZERO(&depositary->rs);
    FD_ZERO(&depositary->ws);
    FD_ZERO(&depositary->es);
    RbTreeIterate(depositary->rbTree->root, EventReInitSingleFd);
}

static void EventHandleCallback(struct RbTreeNode *node) {
    struct EventHandler *handler = (struct EventHandler *) node->data;
    struct EventDepositary *depositary = handler->depositary;
    if (FD_ISSET(handler->fd, &depositary->rs)) {
        printf("depositary address:%lu\n", &depositary->rs);
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
    int ret;
    EventReInitLoop(depositary);
    printf("selected start\n");
    ret = select(FD_SETSIZE, &depositary->rs, &depositary->ws, &depositary->es, depositary->tv);
    printf("selected finished\n");
    if (ret < 0) {
        return -1;
    } else {
        if (ret > 0) {
            RbTreeIterate(depositary->rbTree->root, EventHandleCallback);
        }
        return 1;
    }
}