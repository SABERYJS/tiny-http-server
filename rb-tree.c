//
// Created by Administrator on 2019/6/23.
//

#include "config_header.h"
#include "rb-tree.h"

struct RbTree *RbTreeCreate(int (*compare)(struct RbTreeNode *a, struct RbTreeNode *b),
                            void (*update)(struct RbTreeNode *node, void *data)) {
    struct RbTree *tree = malloc(sizeof(struct RbTree));
    if (!tree) {
        return NULL;
    } else {
        tree->count = 0;
        tree->compare = compare;
        tree->root = NULL;
        tree->update = update;
        return tree;
    }
}

struct RbTreeNode *RbTreeCreateNewNode(struct RbTree *tree, void *data) {
    struct RbTreeNode *node = malloc(sizeof(struct RbTreeNode));
    if (!node) {
        return NULL;
    } else {
        node->data = data;
        node->color = RBTREE_COLOR_RED;
        node->rChild = node->pNode = node->lChild = NULL;
        return node;
    }
}

static struct RbTreeNode *RbTreeLookParentNode(struct RbTree *tree, struct RbTreeNode *node) {
    struct RbTreeNode *tNode = tree->root;
    while (tNode) {
        if (tree->compare(node, tNode) <= 0) {
            if (tNode->lChild) {
                tNode = tNode->lChild;
            } else {
                break;
            }
        } else {
            if (tNode->rChild) {
                tNode = tNode->rChild;
            } else {
                break;
            }
        }
    }
    return tNode;
}

struct RbTreeNode *RbTreeSearch(struct RbTree *tree, void *data) {
    struct RbTreeNode *tmp = malloc(sizeof(struct RbTreeNode));
    if (!tmp) {
        return NULL;
    } else {
        tmp->data = data;
        struct RbTreeNode *tNode = tree->root;
        int ret;
        while (tNode) {
            if ((ret = tree->compare(tmp, tNode)) == 0) {
                return tNode;
            } else {
                if (ret < 0) {
                    if (!tNode->lChild)return NULL;
                    tNode = tNode->lChild;
                } else {
                    if (!tNode->rChild)return NULL;
                    tNode = tNode->rChild;
                }
            }
        }
        return NULL;
    }
}

static void RbTreeHandleDoubleRedCollision(struct RbTreeNode *node) {
    struct RbTreeNode *uncle = RbUncleNode(node);
    struct RbTreeNode *father = RbTreeFather(node);
    if (!uncle || RbNodeColorRed(uncle)) {
        //uncle is null or red
        if (RbTreeIsLeft(node) && RbTreeIsLeft(father)) {
            RbTreeRotateRight1(node);
        } else if (!RbTreeIsLeft(node) && RbTreeIsLeft(father)) {
            RbTreeRotateLeft2(node);
            RbTreeRotateRight1(father);
        } else if (!RbTreeIsLeft(node) && !RbTreeIsLeft(father)) {
            RbTreeRotateLeft1(node);
        } else {
            RbTreeRotateRight2(node);
            RbTreeRotateLeft1(father);
        }
    } else {
        //uncle is red
        RbTreeUncleIsRed(node);
    }
}

static void RbTreeUncleIsRed(struct RbTreeNode *node) {
    RbTreeSetColorBlack(RbTreeFather(node));
    RbTreeSetColorRed(RbTreeGrand(node));
    RbTreeSetColorBlack(RbUncleNode(node));
    if (!RbTreeFather(RbTreeGrand(node))) {
        //iterate over,the whole tree increase a new black node
        RbTreeSetColorBlack(RbTreeGrand(node));
    } else {
        //handle red node collision  upward
        RbTreeHandleDoubleRedCollision(RbTreeGrand(node));
    }
}

static void RbTreeRotateLeft2(struct RbTreeNode *node) {
    struct RbTreeNode *uncle = RbUncleNode(node);
    struct RbTreeNode *greatGrand = RbTreeGreatGrand(node);
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    RbTreeSetParent(node, grand);
    RbTreeSetLeftChild(grand, node);

    RbTreeSetParent(father, node);
    RbTreeSetLeftChild(node, father);

    RbTreeSetRightChild(father, RbTreeLeftChild(node));
    RbTreeSetParent(RbTreeLeftChild(node), father);
}

static void RbTreeRotateLeft1(struct RbTreeNode *node) {
    struct RbTreeNode *greatGrand = RbTreeGreatGrand(node);
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    RbTreeSetLeftChild(father, grand);
    RbTreeSetRightChild(grand, RbTreeLeftChild(father));
    RbTreeSetParent(RbTreeLeftChild(father), grand);
    if (greatGrand) {
        RbTreeSetParent(father, greatGrand);
        if (RbTreeIsLeft(grand)) {
            RbTreeSetLeftChild(greatGrand, father);
        } else {
            RbTreeSetRightChild(greatGrand, father);
        }
    }
    RbTreeSetParent(grand, father);
    RbTreeSetColorRed(grand);
    RbTreeSetColorBlack(father);
}

/**
 * rotate for node deletion when deleted node  is left node and brother`s  right node is not null and  color is red
 * **/
static void RbTreeRotateLeft3(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    RbTreeSetNodeColor(brother, RbTreeNodeColor(father));
    RbTreeSetColorBlack(father);
    RbTreeSetColorBlack(RbTreeRightChild(brother));
    RbTreeSetParent(brother, grand);
    RbTreeSetRightChild(father, RbTreeLeftChild(brother));
    RbTreeSetParent(RbTreeLeftChild(brother), father);
    if (grand) {
        if (RbTreeIsLeft(father)) {
            RbTreeSetLeftChild(grand, brother);
        } else {
            RbTreeSetRightChild(grand, brother);
        }
    }
    RbTreeSetParent(father, brother);
    RbTreeSetLeftChild(brother, father);
}

/**
 * rotate for node deletion when deleted node  is left node and brother`s  left node is not null and  color is red
**/
static void RbTreeRotateRight4(struct RbTreeNode *node) {
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    struct RbTreeNode *left = RbTreeLeftChild(brother);
    RbTreeSetParent(left, father);
    RbTreeSetParent(brother, left);
    RbTreeSetRightChild(left, brother);
    RbTreeSetRightChild(father, left);
    RbTreeSetColorBlack(left);
    RbTreeSetColorRed(brother);
}

/**
 * rotate for node deletion when deleted node  is right node and brother`s  right node is not null and  color is red
**/
static void RbTreeRotateLeft4(struct RbTreeNode *node) {
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    struct RbTreeNode *right = RbTreeLeftChild(node);
    RbTreeSetParent(right, father);
    RbTreeSetParent(brother, right);
    RbTreeSetLeftChild(right, brother);
    RbTreeSetLeftChild(father, right);
    RbTreeSetColorBlack(right);
    RbTreeSetColorRed(brother);
}

/**
 * for deletion and brother is red and deleted node is right
 * **/
static void RbTreeRotateRight5(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    struct RbTreeNode *father = RbTreeFather(node);
    RbTreeSetRightChild(brother, father);
    RbTreeSetLeftChild(father, RbTreeRightChild(brother));
    RbTreeSetParent(RbTreeRightChild(brother), father);
    RbTreeSetParent(brother, grand);
    if (grand) {
        if (RbTreeIsLeft(father)) {
            RbTreeSetLeftChild(grand, brother);
        } else {
            RbTreeSetRightChild(grand, brother);
        }
    }
    RbTreeSetParent(father, brother);
    RbTreeSetColorRed(father);
    RbTreeSetColorBlack(brother);
}

/**
 * for deletion and brother is red and deleted node is left
 * **/
static void RbTreeRotateLeft5(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    struct RbTreeNode *father = RbTreeFather(node);
    RbTreeSetLeftChild(brother, father);
    RbTreeSetRightChild(father, RbTreeLeftChild(brother));
    RbTreeSetParent(RbTreeLeftChild(brother), father);
    RbTreeSetParent(brother, grand);
    if (grand) {
        if (RbTreeIsLeft(father)) {
            RbTreeSetLeftChild(grand, brother);
        } else {
            RbTreeSetRightChild(grand, brother);
        }
    }
    RbTreeSetParent(father, brother);
    RbTreeSetColorRed(father);
    RbTreeSetColorBlack(brother);
}

/**
 * rotate for node deletion when deleted node is right node
 * **/
static void RbTreeRotateRight3(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    RbTreeSetNodeColor(brother, RbTreeNodeColor(father));
    RbTreeSetColorBlack(father);
    RbTreeSetColorBlack(RbTreeLeftChild(brother));
    RbTreeSetParent(brother, grand);
    RbTreeSetLeftChild(father, RbTreeRightChild(brother));
    RbTreeSetParent(RbTreeRightChild(brother), father);
    if (grand) {
        if (RbTreeIsLeft(father)) {
            RbTreeSetLeftChild(grand, brother);
        } else {
            RbTreeSetRightChild(grand, brother);
        }
    }
    RbTreeSetParent(father, brother);
    RbTreeSetRightChild(brother, father);
}


static void RbTreeRotateRight1(struct RbTreeNode *node) {
    struct RbTreeNode *greatGrand = RbTreeGreatGrand(node);
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);

    RbTreeSetLeftChild(grand, RbTreeRightChild(father));
    RbTreeSetParent(RbTreeRightChild(father), grand);
    RbTreeSetRightChild(father, grand);
    if (greatGrand) {
        RbTreeSetParent(father, greatGrand);
        if (RbTreeIsLeft(grand)) {
            RbTreeSetLeftChild(greatGrand, father);
        } else {
            RbTreeSetRightChild(greatGrand, father);
        }
    }
    RbTreeSetParent(grand, father);
    RbTreeSetColorBlack(father);
    RbTreeSetColorRed(grand);
}

static void RbTreeRotateRight2(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    RbTreeSetParent(node, grand);
    RbTreeSetRightChild(node, father);
    RbTreeSetParent(father, node);
    RbTreeSetRightChild(grand, node);
    RbTreeSetLeftChild(father, RbTreeRightChild(node));
    RbTreeSetParent(RbTreeRightChild(node), father);
}

struct RbTreeNode *RbTreeSearchSuccessorNode(struct RbTreeNode *node) {
    struct RbTreeNode *right = RbTreeRightChild(node);
    if (!RbTreeLeftChild(right)) {
        return right;
    } else {
        struct RbTreeNode *tmp = right;
        while (RbTreeLeftChild(tmp)) {
            tmp = RbTreeLeftChild(tmp);
        }
        return tmp;
    }
}


static void RbTreeReplaceEachOther(struct RbTreeNode *target, struct RbTreeNode *replace) {
    size_t len = sizeof(struct RbTreeNode);
    struct RbTreeNode *tmp = malloc(len);
    memcpy(tmp, replace, len);
    memcpy(replace, target, len);
    memcpy(target, tmp, len);
}

struct RbTreeNode *RbTreeInsertNode(struct RbTree *tree, void *data) {
    struct RbTreeNode *nd;
    if ((nd = RbTreeSearch(tree, data))) {
        if (tree->update) {
            tree->update(nd, data);
        }
        return nd;
    } else {
        nd = RbTreeCreateNewNode(tree, data);
    }
    if (!nd) {
        return NULL;
    } else {
        if (RbTreeEmpty(tree)) {
            tree->root = nd;
            RbTreeSetColorBlack(tree->root);
            return nd;
        } else {
            struct RbTreeNode *parent = RbTreeLookParentNode(tree, nd);
            if (tree->compare(nd, parent) <= 0) {
                RbTreeSetLeftChild(parent, nd);
            } else {
                RbTreeSetRightChild(parent, nd);
            }
            if (RbTreeParentColorBlack(nd)) {
                return nd;
            } else {
                RbTreeHandleDoubleRedCollision(nd);
            }
        }
    }
}

/**
 * delete case 1
 * deleted node  has no children and color is red
 * */
static void RbTreeDeleteCase1(struct RbTreeNode *node) {
    struct RbTreeNode *father = RbTreeFather(node);
    //father must exist
    if (RbTreeIsLeft(node)) {
        RbTreeSetLeftChild(father, NULL);
    } else {
        RbTreeSetRightChild(father, NULL);
    }
    RbTreeSetParent(node, NULL);
}

/**
 * deleted node  has one child and color is black,we can infer the only child is right node and color is Red
 * **/
static void RbTreeDeleteCase2(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *father = RbTreeFather(node);
    RbNodeColorBlack(RbTreeRightChild(node));
    if (RbNodeIsRoot(node)) {
        tree->root = RbTreeRightChild(node);
    } else {
        if (RbTreeIsLeft(node)) {
            RbTreeSetLeftChild(RbTreeFather(father), RbTreeRightChild(node));
        } else {
            RbTreeSetRightChild(RbTreeFather(father), RbTreeRightChild(node));
        }
    }
    RbTreeSetParent(RbTreeRightChild(node), RbTreeFather(father));
}

/**
 * deleted child has two child
 * */
static void RbTreeDeleteCase3(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *successor = RbTreeSearchSuccessorNode(node);
    //nodes replace each other firstly
    RbTreeReplaceEachOther(node, successor);
    RbTreeDeleteNode(tree, successor);
}

static void RbTreeDeleteCase4(struct RbTreeNode *node, struct RbTree *tree) {
    //brother must exist,But color is uncertain
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    if (RbNodeColorBlack(brother)) {
        struct RbTreeNode *right = RbTreeRightChild(node);
        struct RbTreeNode *left = RbTreeLeftChild(node);
        if (right && RbTreeParentColorRed(right) && RbTreeIsLeft(node)) {
            RbTreeRotateLeft3(node);
        } else if (left && RbNodeColorRed(left) && RbTreeIsLeft(node)) {
            RbTreeRotateRight4(node);
            RbTreeRotateLeft3(node);
        } else if (right && RbTreeParentColorRed(right) && !RbTreeIsLeft(node)) {
            RbTreeRotateLeft4(node);
            RbTreeRotateRight3(node);
        } else if (left && RbNodeColorRed(left) && !RbTreeIsLeft(node)) {
            RbTreeRotateRight3(node);
        } else {
            RbTreeDeleteCase4(RbTreeFather(node), tree);
        }
    } else {
        //brother is red
        if (RbTreeIsLeft(node)) {
            RbTreeRotateLeft5(node);
        } else {
            RbTreeRotateRight5(node);
        }
        RbTreeDeleteCase4(node, tree);
    }
}

static void RbTreeDeleteCase5(struct RbTreeNode *node, struct RbTree *tree) {

}

void RbTreeDeleteNode(struct RbTree *tree, struct RbTreeNode *node) {
    if (RbTreeEmpty(tree)) {
        return;
    }
    struct RbTreeNode *father = RbTreeFather(node);
    if (RbNodeNoChildren(node) && RbNodeColorRed(node)) {
        RbTreeDeleteCase1(node);
    } else if (RbTreeHasOnlyOneRightChild(node)) {
        RbTreeDeleteCase2(node, tree);
    } else if (RbTreeHasTwoChildren(node)) {
        RbTreeDeleteCase3(node, tree);
    } else {
        //node is black and no children
        RbTreeDeleteCase4(node, tree);
    }
}
