//
// Created by Administrator on 2019/6/18 0018.
//

#ifndef STL_CLONE_STACK_TEST_H
#define STL_CLONE_STACK_TEST_H

#include "../stack.h"
#include "../unit.h"

void testStack() {
    struct Stack *s = StackCreate();
    srand(time(NULL));
    for (int i = 0; i < 10; i++) {
        struct Unit *u = malloc(sizeof(struct Unit));
        u->a = rand();
        printf("push value:%d\n", u->a);
        StackPush(s, u);
    }

    printf("%d\n", STackSize(s));
    printf("top:%d\n", ((struct Unit *) StackTop(s))->a);
    StackPop(s);
    printf("top:%d\n", ((struct Unit *) StackTop(s))->a);
    struct Unit *p;
    while ((p = (struct Unit *) StackNext(s))) {
        printf("value:%d\n", p->a);
    }
    printf("%d\n", STackSize(s));
}

#endif //STL_CLONE_STACK_TEST_H
