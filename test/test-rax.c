//
// Created by Administrator on 2019/6/29.
//

#include "radix-tree.h"
#include <stdlib.h>
#include <stdio.h>
#include "unit.h"

int main() {
    struct rax *rx = RadixTreeCreate();
    struct Unit *u = malloc(sizeof(struct Unit));
    u->a = 100;
    RadixTreeInsert(rx, "name", 4, u, NULL);
    struct Unit *tp = (struct Unit *) RadixTreeSearch(rx, "name", 4);
    printf("%d\n", tp->a);
}