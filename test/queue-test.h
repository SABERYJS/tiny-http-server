//
// Created by Administrator on 2019/6/19.
//

#ifndef STL_CLONE_QUEUE_TEST_H
#define STL_CLONE_QUEUE_TEST_H

#include "../src/algorithm/config_header.h"
#include "../src/algorithm/queue.h"
#include "../src/algorithm/unit.h"

void testQueue() {
    struct Queue *queue = QueueCreate();
    int i;
    for (i = 0; i < 10; ++i) {
        struct Unit *u = malloc(sizeof(struct Unit));
        u->a = i;
        QueuePush(queue, u);
    }

    printf("size:%d\n", QueueSize(queue));
    struct Unit *tp;
    while ((tp = (struct Unit *) QueueNext(queue))) {
        printf("value  is:%d\n", tp->a);
    }
    printf("size:%d\n", QueueSize(queue));

}

#endif //STL_CLONE_QUEUE_TEST_H
