/*Copyright (c) 2019 , saberyjs@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. All advertising materials mentioning features or use of this software
   must display the following acknowledgement:
   This product includes software developed by the <organization>.
4. Neither the name of the <organization> nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef STL_CLONE_RB_TREE_H
#define STL_CLONE_RB_TREE_H

#include "config_header.h"
#include "unit.h"


#define RBTREE_COLOR_BLACK 1
#define RBTREE_COLOR_RED 2


struct RbTreeNode {
    void *data;
    int color;
#ifdef STL_CLONE_UNIT_H
    int key;
#endif
    struct RbTreeNode *lChild;
    struct RbTreeNode *rChild;
    struct RbTreeNode *pNode;
};

struct RbTree {
    struct RbTreeNode *root;
    size_t count;

    int (*compare)(struct RbTreeNode *a, struct RbTreeNode *b);

    void (*update)(struct RbTreeNode *node, void *data);

    void (*clear)(struct RbTreeNode *node);
};

#ifdef  STL_CLONE_UNIT_H
#define RBTREE_LOG_OPEN

static struct Unit *RbTreeNodeParse(struct RbTreeNode *node) {
    return (struct Unit *) node->data;
}

static int RbTreeNodeValue(struct RbTreeNode *node) {
    if (!node->data) {
        printf("data is null\n");
        return 0;
    }
    struct Unit *u = (struct Unit *) node->data;
    return u->a;
}

#endif


#define RbNodeColorBlack(node) (node->color==RBTREE_COLOR_BLACK)
#define RbNodeColorRed(node) (node->color==RBTREE_COLOR_RED)

#define RbTreeFather(node) (node->pNode)
#define RbTreeGrand(node) (node->pNode->pNode)
#define RbNodeNoChildren(node) (!node->lChild && !node->rChild)
#define RbUncleNode(node) ((RbTreeFather(node)==RbTreeGrand(node)->lChild)?(RbTreeGrand(node)->rChild):(RbTreeGrand(node)->lChild))
#define RbTreeGreatGrand(node) ((RbTreeGrand(node))->pNode)
#define RbTreeLeftChild(node) (node->lChild)
#define RbTreeRightChild(node) (node->rChild)
#define RbNodeIsRoot(node)  (!node->pNode)
#define RbTreeSize(tree) (tree->count)
#define RbTreeEmpty(tree) (RbTreeSize(tree)==0)

#define RbTreeParentColor(parent, c) ((parent->color)==c)
#define RbTreeParentColorRed(parent) (RbTreeParentColor(parent,RBTREE_COLOR_RED))
#define RbTreeParentColorBlack(parent) (RbTreeParentColor(parent,RBTREE_COLOR_BLACK))
#define RbTreeIsLeft(node) (node==node->pNode->lChild)
#define RbTreeHasTwoChildren(node) (node->lChild && node->rChild)
#define RbTreeNodeColor(node) (node->color)
#define RbTreeNodeData(node) (node->data)
#define RbTreeBrotherNode(node) (RbTreeIsLeft(node)?RbTreeRightChild(RbTreeFather(node)):RbTreeLeftChild(RbTreeFather(node)))

#ifdef RBTREE_LOG_OPEN

static void RbTreePrintNode(struct RbTreeNode *node) {
    struct Unit *u = (struct Unit *) node->data;
    printf("value:%d,color:%s,side:%s,parent node is %d\n",
           u->a,
           node->color == RBTREE_COLOR_BLACK ? "black" : "red",
           RbNodeIsRoot(node) ? "root" : (RbTreeIsLeft(node) ? "left" : "right"),
           !RbNodeIsRoot(node) ? RbTreeNodeValue(RbTreeFather(node)) : 0);
}

#endif

static inline void RbTreeSetColorRed(struct RbTreeNode *node) {
    node->color = RBTREE_COLOR_RED;
}

static inline void RbTreeSetColorBlack(struct RbTreeNode *node) {
    node->color = RBTREE_COLOR_BLACK;
}

static inline void RbTreeSetLeftChild(struct RbTreeNode *parent, struct RbTreeNode *child) {
    parent->lChild = child;
}

static inline void RbTreeSetRightChild(struct RbTreeNode *parent, struct RbTreeNode *child) {
    parent->rChild = child;
}

static void RbTreeSetParent(struct RbTreeNode *node, struct RbTreeNode *parent) {
    node->pNode = parent;
}

static inline void RbTreeSetNodeColor(struct RbTreeNode *node, int color) {
    node->color = color;
}

static inline void RbTreeSetNodeData(struct RbTreeNode *node, void *data) {
    node->data = data;
}

/**
 * if node has only one child,so it`s color must be black and child is red,but which side the child is at is uncertain
 * **/
static inline int RbTreeHasOneChild(struct RbTreeNode *node) {
    return (RbTreeLeftChild(node) && !RbTreeRightChild(node)) || (!RbTreeLeftChild(node) && RbTreeRightChild(node));
}

static inline void RbTreeIncreaseNodeCount(struct RbTree *tree) {
    tree->count++;
}

static inline void RbTreeDecreaseNodeCount(struct RbTree *tree) {
    tree->count--;
}

static inline void RbTreeClearLeftChild(struct RbTreeNode *node) {
    RbTreeSetLeftChild(node, NULL);
}

static inline void RbTreeClearRightChild(struct RbTreeNode *node) {
    RbTreeSetRightChild(node, NULL);
}

static inline void RbTreeClearParent(struct RbTreeNode *node) {
    RbTreeSetParent(node, NULL);
}

static inline void RbTreeSetRoot(struct RbTree *tree, struct RbTreeNode *node) {
    tree->root = node;
}

static void RbTreeClearNode(struct RbTreeNode *node, struct RbTree *tree);

struct RbTree *RbTreeCreate(int (*compare)(struct RbTreeNode *a, struct RbTreeNode *b),
                            void (*update)(struct RbTreeNode *node, void *data),
                            void (*clear)(struct RbTreeNode *node));

struct RbTreeNode *RbTreeCreateNewNode(struct RbTree *tree, void *data);

struct RbTreeNode *RbTreeInsertNode(struct RbTree *tree, void *data);

static void RbTreeReplaceEachOther(struct RbTreeNode *target, struct RbTreeNode *replace);

struct RbTreeNode *RbTreeSearchSuccessorNode(struct RbTreeNode *node);


static struct RbTreeNode *RbTreeLookParentNode(struct RbTree *tree, struct RbTreeNode *node);

static void RbTreeRotateRight1(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeRotateRight2(struct RbTreeNode *node);

static void RbTreeRotateRight3(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeRotateRight4(struct RbTreeNode *node);

static void RbTreeRotateRight5(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeRotateLeft1(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeRotateLeft2(struct RbTreeNode *node);

static void RbTreeRotateLeft3(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeRotateLeft4(struct RbTreeNode *node);

static void RbTreeRotateLeft5(struct RbTreeNode *node, struct RbTree *tree);

struct RbTreeNode *RbTreeSearch(struct RbTree *tree, void *data);

static void RbTreeUncleIsRed(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeHandleDoubleRedCollision(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase1(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase2(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase3(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase4(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase5(struct RbTreeNode *node, struct RbTree *tree);

void RbTreeDeleteNode(struct RbTree *tree, struct RbTreeNode *node);

void RbTreeIterate(struct RbTreeNode *root, void(*callback)(struct RbTreeNode *node));

int RbTreeBlackCount(struct RbTree *tree);

void RbTreeValidate(struct RbTreeNode *tree, int stl, int count);

#endif //STL_CLONE_RB_TREE_H
