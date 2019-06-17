//
// Created by Administrator on 2019/6/16.
//

#ifndef C___TREE_H
#define C___TREE_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

struct TreeNode {
    void *data;
    struct TreeNode *lNode;
    struct TreeNode *rNode;
    struct TreeNode *pNode;
};

struct Tree {
    struct TreeNode *root;
    int count;

    int (*compare)(struct TreeNode *node, void *data);

    void (*clear)(struct TreeNode *node);
};


struct Tree *createTree(int(*compare)(struct TreeNode *node, void *data), void(*clear)(struct TreeNode *node));

struct TreeNode *insertNode(struct Tree *tree, void *data);


void deleteNode(struct Tree *tree, struct TreeNode *root, struct TreeNode *node);

struct TreeNode *search(struct Tree *tree, struct TreeNode *root, void *data);

void clearTree(struct Tree *tree, struct TreeNode *root);

void iterateTree(struct Tree *tree, struct TreeNode *root, void(*callback)(struct TreeNode *node));


#endif //C___TREE_H
