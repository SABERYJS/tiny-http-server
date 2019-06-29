//
// Created by Administrator on 2019/6/17.
//

#include "../src/algorithm/tree.h"
#include "../src/algorithm/unit.h"

int compareTree(struct TreeNode *node, void *data) {
    struct Unit *u = (struct Unit *) data;
    int ret = u->a - ((struct Unit *) node->data)->a;
    return ret > 0 ? -1 : (ret == 0 ? 0 : 1);
}

void clearTreeNode(struct TreeNode *node) {}

void printNode(struct TreeNode *node) {
    printf("%d\n", ((struct Unit *) node->data)->a);
}


void testarray() {
    printf("%d\n", sizeof(struct TreeNode));
    struct Tree *tree = createTree(compareTree, clearTreeNode);
    int a[10] = {100, 29, 34, 78, 12, 9, 46, 200, 176, 38};
    struct TreeNode *uu, *up;
    for (int i = 0; i < 10; i++) {
        struct Unit *u = malloc(sizeof(struct Unit));
        u->a = a[i];
        uu = insertNode(tree, u);
        if (a[i] == 34) {
            up = uu;
        }
    }
    printf("num:%d\n", tree->count);
    iterateTree(tree, tree->root, printNode);
    printf("--------------\n");
    deleteNode(tree, tree->root, up);
    iterateTree(tree, tree->root, printNode);
}