//
// Created by Administrator on 2019/6/18 0018.
//

#ifndef STL_CLONE_QUEUE_H
#define STL_CLONE_QUEUE_H

#include "config_header.h"
#include "list.h"

struct Queue {
    struct List *list;
    size_t count;
};

struct Queue *QueueCreate();


int QueuePush(struct Queue *queue, void *data);

void QueueNext(struct Queue *queue);

void QueueFront(struct Queue *queue);

void QueueBack(struct Queue *queue);


#define QueueSize (q) (q->count)
#define QueueEmpty(q) (q->count==0)

#endif //STL_CLONE_QUEUE_H
