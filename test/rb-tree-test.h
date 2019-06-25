//
// Created by Administrator on 2019/6/25.
//

#ifndef STL_CLONE_RB_TREE_TEST_H
#define STL_CLONE_RB_TREE_TEST_H

#include "../config_header.h"
#include "../rb-tree.h"
#include "../unit.h"

int Compare(struct RbTreeNode *a, struct RbTreeNode *b) {
    struct Unit *at = (struct Unit *) a->data;
    struct Unit *bt = (struct Unit *) b->data;
    return at->a > bt->a ? 1 : (at->a == bt->a ? 0 : -1);
}

void printRbTreeNode(struct RbTreeNode *node) {
    struct Unit *u = (struct Unit *) node->data;
    printf("value:%d,color:%s,side:%s,parent node is %d\n",
           u->a,
           node->color == RBTREE_COLOR_BLACK ? "black" : "red",
           RbNodeIsRoot(node) ? "root" : (RbTreeIsLeft(node) ? "left" : "right"),
           !RbNodeIsRoot(node) ? RbTreeNodeValue(RbTreeFather(node)) : 0);
}

void test_rbtree() {
    struct RbTree *tree = RbTreeCreate(Compare, NULL, NULL);
    int inertted[] = {1000, 500, 200, 100, 45, 78, 89, 789, 326, 520, 890, 903, 66, 98, 146, 210, 402, 682, 900, 970,
                      288, 564, 156, 998, 8, 6, 4, 18, 14, 600};
    for (int i = 0; i < sizeof(inertted) / sizeof(int); ++i) {
        struct Unit *tmp = malloc(sizeof(struct Unit));
        tmp->a = inertted[i];
        RbTreeInsertNode(tree, tmp);
        /*if (inertted[i] == 89) {
            //RbTreeIterate(tree->root, printRbTreeNode);
            exit(0);
        }*/
    }
    printf("nodes count :%d\n", RbTreeSize(tree));
    RbTreeIterate(tree->root, printRbTreeNode);
}

#endif //STL_CLONE_RB_TREE_TEST_H
