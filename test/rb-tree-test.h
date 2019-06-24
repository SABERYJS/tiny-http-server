//
// Created by Administrator on 2019/6/25.
//

#ifndef STL_CLONE_RB_TREE_TEST_H
#define STL_CLONE_RB_TREE_TEST_H

#include "../rb-tree.h"
#include "../unit.h"

int Compare(struct RbTreeNode *a, struct RbTreeNode *b) {
    struct Unit *at = (struct Unit *) a->data;
    struct Unit *bt = (struct Unit *) b->data;
    return at->a > bt->a ? 1 : (at->a == bt->a ? 0 : 1);
}

void printRbTreeNode(struct RbTreeNode *node) {
    struct Unit *u = (struct Unit *) node->data;
    printf("value:%d,color:%s,side:%s\n", u->a, node->color == RBTREE_COLOR_BLACK ? "black" : "red",
           RbTreeIsLeft(node) ? "left" : "right");
}

void test_rbtree() {
    struct RbTree *tree = RbTreeCreate(Compare, NULL, NULL);
    int inertted[] = {1000, 500, 200, 100, 45, 78, 89, 789, 326, 520, 890, 903, 66, 98, 146, 210, 402, 682, 900, 970,
                      288, 564, 156, 998, 8, 6, 4, 18, 14, 600};
    printf("%d\n", sizeof(inertted) / sizeof(int));
    for (int i = 0; i < sizeof(inertted) / sizeof(int); ++i) {
        struct Unit *tmp = malloc(sizeof(struct Unit));
        tmp->a = inertted[i];
        RbTreeInsertNode(tree, tmp);
    }
    RbTreeIterate(tree->root, printRbTreeNode);
}

#endif //STL_CLONE_RB_TREE_TEST_H
