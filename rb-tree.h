//
// Created by Administrator on 2019/6/23.
//

#ifndef STL_CLONE_RB_TREE_H
#define STL_CLONE_RB_TREE_H

#define RBTREE_COLOR_BLACK 1
#define RBTREE_COLOR_RED 2


struct RbTreeNode {
    void *data;
    int color;
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


#define RbNodeColorBlack(node) (node->color==RBTREE_COLOR_BLACK)
#define RbNodeColorRed(node) (node->color==RBTREE_COLOR_RED)

#define RbNodeNoChildren(node) (!node->lChild && !node->rChild)
#define RbUncleNode(node) (node==node->pNode->lChild?node->pNode->rChild:node->pNode->lChild)
#define RbTreeGrand(node) (node->pNode->pNode)
#define RbTreeGreatGrand(node) ((RbTreeGrand(node))->pNode)
#define RbTreeFather(node) (node->pNode)
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

static inline int RbTreeHasOneChild(struct RbTreeNode *node) {
    return (RbTreeLeftChild(node) && !RbTreeRightChild(node)) || (!RbTreeLeftChild(node) && RbTreeRightChild(node));
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

static void RbTreeRotateRight1(struct RbTreeNode *node);

static void RbTreeRotateRight2(struct RbTreeNode *node);

static void RbTreeRotateRight3(struct RbTreeNode *node);

static void RbTreeRotateRight4(struct RbTreeNode *node);

static void RbTreeRotateRight5(struct RbTreeNode *node);

static void RbTreeRotateLeft1(struct RbTreeNode *node);

static void RbTreeRotateLeft2(struct RbTreeNode *node);

static void RbTreeRotateLeft3(struct RbTreeNode *node);

static void RbTreeRotateLeft4(struct RbTreeNode *node);

static void RbTreeRotateLeft5(struct RbTreeNode *node);

struct RbTreeNode *RbTreeSearch(struct RbTree *tree, void *data);

static void RbTreeUncleIsRed(struct RbTreeNode *node);

static void RbTreeHandleDoubleRedCollision(struct RbTreeNode *node);

static void RbTreeDeleteCase1(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase2(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase3(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase4(struct RbTreeNode *node, struct RbTree *tree);

static void RbTreeDeleteCase5(struct RbTreeNode *node, struct RbTree *tree);

void RbTreeDeleteNode(struct RbTree *tree, struct RbTreeNode *node);

void RbTreeIterate(struct RbTreeNode *root, void(*callback)(struct RbTreeNode *node));

#endif //STL_CLONE_RB_TREE_H
