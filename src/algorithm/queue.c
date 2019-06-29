//
// Created by Administrator on 2019/6/18 0018.
//

#include "queue.h"


struct Queue *QueueCreate() {
    struct Queue *queue = malloc(sizeof(struct Queue));
    if (!queue) {
        return NULL;
    } else {
        queue->list = createList(QueueCompareMethod, QueueClearMethod);
        if (!queue->list) {
            free(queue);
            return NULL;
        } else {
            queue->count = 0;
            return queue;
        }
    }
}


int QueuePush(struct Queue *queue, void *data) {
    int ret = appendNode(queue->list, data);
    if (ret == 0) {
        queue->count++;
    }
    return ret;
}


void *QueueNext(struct Queue *queue) {
    if (QueueEmpty(queue)) {
        return NULL;
    } else {
        void *data = queue->list->header->data;
        removeNode(queue->list, queue->list->header);
        queue->count--;
        return data;
    }
}

void *QueueFront(struct Queue *queue) {
    if (QueueEmpty(queue)) {
        return NULL;
    } else {
        return queue->list->header->data;
    }
}

void *QueueBack(struct Queue *queue) {
    if (QueueEmpty(queue)) {
        return NULL;
    } else {
        return queue->list->tail->data;
    }
}
