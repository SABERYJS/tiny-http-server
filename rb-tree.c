//
// Created by Administrator on 2019/6/23.
//

#include "rb-tree.h"

/**
 * create Rea Black Tree
 * @param compare used for search Tree and compare node
 * @param update  used for update node
 * @attention  if failed return NULL
 * **/
struct RbTree *RbTreeCreate(int (*compare)(struct RbTreeNode *a, struct RbTreeNode *b),
                            void (*update)(struct RbTreeNode *node, void *data),
                            void (*clear)(struct RbTreeNode *node)) {
    struct RbTree *tree = malloc(sizeof(struct RbTree));
    if (!compare) {
        //compare method is necessary
        return NULL;
    }
    if (!tree) {
        return NULL;
    } else {
        tree->count = 0;
        tree->compare = compare;
        tree->root = NULL;
        tree->update = update;
        tree->clear = clear;
        return tree;
    }
}

/**
 * create new node
 * @attention color of inserted node must be  red
 * **/
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

void RbTreeIterate(struct RbTreeNode *root, void(*callback)(struct RbTreeNode *)) {
    struct RbTreeNode *node = root;
    callback(node);
    if (RbTreeLeftChild(node)) {
        RbTreeIterate(RbTreeLeftChild(node), callback);
    }
    if (RbTreeRightChild(node)) {
        RbTreeIterate(RbTreeRightChild(node), callback);
    }
}

/**
 * look for node insert position
 * @attention only used by node insertion
 * **/
static struct RbTreeNode *RbTreeLookParentNode(struct RbTree *tree, struct RbTreeNode *node) {
    struct RbTreeNode *tNode = tree->root;
    while (tNode) {
        if (tree->compare(node, tNode) < 0) {
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

/**
 * look for node from Tree,if found return ,if not found return NULL
 * **/
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

static void RbTreeClearNode(struct RbTreeNode *node, struct RbTree *tree) {
    RbTreeSetLeftChild(node, NULL);
    RbTreeSetRightChild(node, NULL);
    RbTreeSetParent(node, NULL);
    if (tree->clear) {
        tree->clear(node);
    }
    free(node);
}

/**
 * handle double red node
 * **/
static void RbTreeHandleDoubleRedCollision(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *uncle = RbUncleNode(node);
    struct RbTreeNode *father = RbTreeFather(node);
    //if parent  is red,so grand  is exist
    //but  the existence and color of uncle is uncertain
    //N stands for current inserted node
    //P stands for parent node
    //U stands for uncle node
    //L stands for node is left  child
    //R stands for node is right child
    if (!uncle || RbNodeColorBlack(uncle)) {
        if (RbTreeIsLeft(node) && RbTreeIsLeft(father)) {
            //N(L),P(L)
            RbTreeRotateRight1(node, tree);
        } else if (!RbTreeIsLeft(node) && RbTreeIsLeft(father)) {
            //N(R),P(L)
            RbTreeRotateLeft2(node);
            //rotate father,because father is  left child  of  N now
            RbTreeRotateRight1(father, tree);
        } else if (!RbTreeIsLeft(node) && !RbTreeIsLeft(father)) {
            //N(R),P(R)
            RbTreeRotateLeft1(node, tree);
        } else {
            //N(L),P(R)
            RbTreeRotateRight2(node);
            //now father is child of child
            RbTreeRotateLeft1(father, tree);
        }
    } else {
        //uncle is red
        RbTreeUncleIsRed(node, tree);
    }
}


/**
 * uncle node is red when node insertion
 * **/
static void RbTreeUncleIsRed(struct RbTreeNode *node, struct RbTree *tree) {
#ifdef RBTREE_LOG_OPEN
    printf("uncle[%d] color is red,so change grand color to red\n", RbTreeNodeValue(RbUncleNode(node)));
#endif
    RbTreeSetColorBlack(RbTreeFather(node));
    RbTreeSetColorRed(RbTreeGrand(node));
    RbTreeSetColorBlack(RbUncleNode(node));
    if (!RbTreeFather(RbTreeGrand(node))) {
#ifdef RBTREE_LOG_OPEN
        printf("because great grand does not exist,so node[%d] is root,just modify  color to black\n",
               RbTreeNodeValue(RbTreeGrand(node)));
#endif
        //iterate finished,in fact the whole tree increase a new black node
        RbTreeSetColorBlack(RbTreeGrand(node));
    } else {
#ifdef RBTREE_LOG_OPEN
        printf("because great grand is not null,so iterate upward, new node [%d]\n",
               RbTreeNodeValue(RbTreeGrand(node)));
#endif
        if (RbNodeColorBlack(RbTreeGreatGrand(node))) {
#ifdef RBTREE_LOG_OPEN
            printf("because great grand is black,so tree is in balance,iterate  finished\n");
#endif
            return;
        }
        //because  grand  become red ,so it is possible that great grand is also red,then we must iterate again,but iterated  target is grand
        RbTreeHandleDoubleRedCollision(RbTreeGrand(node), tree);
    }
}

/**
 * rotate for  node insertion
 * when  N is right  side,but  father is left side
 * **/
static void RbTreeRotateLeft2(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);

    if (RbTreeLeftChild(node)) {
        RbTreeSetRightChild(father, RbTreeLeftChild(node));
        RbTreeSetParent(RbTreeLeftChild(node), father);
    } else {
        RbTreeClearRightChild(father);
    }

    RbTreeSetParent(node, grand);
    RbTreeSetLeftChild(grand, node);

    RbTreeSetParent(father, node);
    RbTreeSetLeftChild(node, father);

#ifdef RBTREE_LOG_OPEN
    printf("rotate-left-2 finished,node[%d],parent[%d]\n", RbTreeNodeValue(node), RbTreeNodeValue(father));
#endif
}

/**
 * rotate for node insertion
 * when N and father are both  right side
 * **/
static void RbTreeRotateLeft1(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *greatGrand = RbTreeGreatGrand(node);
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    if (RbTreeLeftChild(father)) {
        RbTreeSetRightChild(grand, RbTreeLeftChild(father));
        RbTreeSetParent(RbTreeLeftChild(father), grand);
    } else {
        RbTreeClearRightChild(grand);
    }
    if (greatGrand) {
        RbTreeSetParent(father, greatGrand);
        if (RbTreeIsLeft(grand)) {
            RbTreeSetLeftChild(greatGrand, father);
        } else {
            RbTreeSetRightChild(greatGrand, father);
        }
    } else {
        RbTreeClearParent(father);
        RbTreeSetRoot(tree, father);
    }
    RbTreeSetLeftChild(father, grand);
    RbTreeSetParent(grand, father);
    RbTreeSetColorRed(grand);
    RbTreeSetColorBlack(father);
#ifdef RBTREE_LOG_OPEN
    printf("rotate-left-1 finished,node[%d],parent[%d]\n", RbTreeNodeValue(node), RbTreeNodeValue(father));
#endif
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

    RbTreeSetLeftChild(brother, RbTreeRightChild(left));
    RbTreeSetParent(RbTreeRightChild(left), brother);
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

    RbTreeSetRightChild(brother, RbTreeLeftChild(right));
    RbTreeSetParent(RbTreeLeftChild(right), brother);
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

/**
 *  rotate for  node insertion
 *  case :C  and P is  on the left side
 * **/
static void RbTreeRotateRight1(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *greatGrand = RbTreeGreatGrand(node);
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);

    if (RbTreeRightChild(father)) {
        RbTreeSetLeftChild(grand, RbTreeRightChild(father));
        RbTreeSetParent(RbTreeRightChild(father), grand);
    } else {
        RbTreeClearLeftChild(grand);
    }
    RbTreeSetRightChild(father, grand);
    if (greatGrand) {
        RbTreeSetParent(father, greatGrand);
        if (RbTreeIsLeft(grand)) {
            RbTreeSetLeftChild(greatGrand, father);
        } else {
            RbTreeSetRightChild(greatGrand, father);
        }
    } else {
#ifdef RBTREE_LOG_OPEN
        printf("new root node[%d]\n", RbTreeNodeValue(father));
#endif
        RbTreeClearParent(father);
        RbTreeSetRoot(tree, father);
    }
    RbTreeSetParent(grand, father);
    RbTreeSetColorBlack(father);
    RbTreeSetColorRed(grand);
#ifdef RBTREE_LOG_OPEN
    printf("rotate-right-1 finished,current[%d] and parent[%d]\n", RbTreeNodeValue(node), RbTreeNodeValue(father));
#endif
}

static void RbTreeRotateRight2(struct RbTreeNode *node) {
    struct RbTreeNode *grand = RbTreeGrand(node);
    struct RbTreeNode *father = RbTreeFather(node);
    if (RbTreeRightChild(node)) {
        RbTreeSetLeftChild(father, RbTreeRightChild(node));
        RbTreeSetParent(RbTreeRightChild(node), father);
    } else {
        RbTreeClearLeftChild(father);
    }
    RbTreeSetParent(node, grand);
    RbTreeSetRightChild(node, father);
    RbTreeSetParent(father, node);
    RbTreeSetRightChild(grand, node);
#ifdef RBTREE_LOG_OPEN
    printf("rotate-right-2 finished,node[%d],parent[%d]\n", RbTreeNodeValue(node), RbTreeNodeValue(father));
#endif
}

/**
 * look for successor node
 * **/
struct RbTreeNode *RbTreeSearchSuccessorNode(struct RbTreeNode *node) {
    struct RbTreeNode *right = RbTreeRightChild(node);
    struct RbTreeNode *tmp = right;
    while (RbTreeLeftChild(tmp)) {
        tmp = RbTreeLeftChild(tmp);
    }
    return tmp;
}


/**
 * only used for node deletion，deleted node  and successor node exchange
 * **/
static void RbTreeReplaceEachOther(struct RbTreeNode *target, struct RbTreeNode *replace) {
    size_t len = sizeof(struct RbTreeNode);
    struct RbTreeNode *tmp = malloc(len);
    memcpy(tmp, replace, len);
    memcpy(replace, target, len);
    memcpy(target, tmp, len);
}

/**
 * insert node
 * **/
struct RbTreeNode *RbTreeInsertNode(struct RbTree *tree, void *data) {
    struct RbTreeNode *nd;
    //search tree firstly,if node exist and update method is defined ,then update ,otherwise return it directly
    if ((nd = RbTreeSearch(tree, data))) {
#ifdef RBTREE_LOG_OPEN
        printf("node with value[%d] exist\n", RbTreeNodeValue(nd));
#endif
        if (tree->update) {
            tree->update(nd, data);
        }
        return nd;
    } else {
        //create a new node
        nd = RbTreeCreateNewNode(tree, data);
    }
    if (!nd) {
        return NULL;
    } else {
        if (RbTreeEmpty(tree)) {
            //if tree is empty，set node color  black and then return
            //the tree is in a state of balance
            tree->root = nd;
            RbTreeSetColorBlack(tree->root);
            RbTreeIncraseNodeCount(tree);
#ifdef RBTREE_LOG_OPEN
            printf("new node:%d\n", RbTreeNodeValue(nd));
#endif
            return nd;
        } else {
            //look parent node to insert
            //operation is the same as  normal binary tree
            RbTreeIncraseNodeCount(tree);
            struct RbTreeNode *parent = RbTreeLookParentNode(tree, nd);
#ifdef RBTREE_LOG_OPEN
            int left = 0;
#endif
            if (tree->compare(nd, parent) <= 0) {
#ifdef RBTREE_LOG_OPEN
                left = 1;
#endif
                RbTreeSetLeftChild(parent, nd);
            } else {
                RbTreeSetRightChild(parent, nd);
            }
            RbTreeSetParent(nd, parent);
#ifdef RBTREE_LOG_OPEN
            printf("node[%d] found parent[%d],and act as %s node\n", RbTreeNodeValue(nd), RbTreeNodeValue(parent),
                   left ? "left" : "right");
#endif
            if (RbNodeColorBlack(parent)) {
                //if parent color is  black ,then return inserted node ,the tree is in a state of balance
#ifdef  RBTREE_LOG_OPEN
                printf("because parent  color of inserted node is black,so insert finished\n");
#endif
                return nd;
            } else {
#ifdef RBTREE_LOG_OPEN
                printf("parent color is red, collision  happened\n");
#endif
                //parent node color is red,so  rule is broken
                RbTreeHandleDoubleRedCollision(nd, tree);
            }
        }
    }
}

/**
 * delete case 1
 * deleted node  has no children and color is red
 * */
static void RbTreeDeleteCase1(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *father = RbTreeFather(node);
    //father must exist
    if (RbTreeIsLeft(node)) {
        RbTreeSetLeftChild(father, NULL);
    } else {
        RbTreeSetRightChild(father, NULL);
    }
    RbTreeClearNode(node, tree);
}

/**
 * deleted node  has one child and color is black,we can infer the only child is Red
 * but left or right is uncertain
 * **/
static void RbTreeDeleteCase2(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *child = RbTreeLeftChild(node) ? RbTreeLeftChild(node) : RbTreeRightChild(node);
    RbTreeSetColorBlack(child);

    if (RbNodeIsRoot(node)) {
        tree->root = child;
    } else {
        if (RbTreeIsLeft(node)) {
            RbTreeSetLeftChild(father, child);
        } else {
            RbTreeSetRightChild(father, child);
        }
    }
    RbTreeSetParent(child, father);
    RbTreeClearNode(node, tree);
}

/**
 * deleted child has two child,we can transform the case
 * */
static void RbTreeDeleteCase3(struct RbTreeNode *node, struct RbTree *tree) {
    struct RbTreeNode *father = RbTreeFather(node);
    struct RbTreeNode *successor = RbTreeSearchSuccessorNode(node);
    //nodes replace each other firstly
    //RbTreeReplaceEachOther(node, successor);
    struct RbTreeNode *replace = malloc(sizeof(struct RbTreeNode));
    if (!replace) {
        return;
    }
    //copy node
    memcpy(replace, node, sizeof(struct RbTreeNode));
    if (RbTreeFather(node)) {
        if (RbTreeIsLeft(node)) {
            RbTreeSetLeftChild(RbTreeFather(node), replace);
        } else {
            RbTreeSetRightChild(RbTreeFather(node), replace);
        }
    }
    RbTreeSetNodeData(replace, RbTreeNodeData(node));
    //so now question is transformed to delete successor,but notice we do not care about content of the successor,just delete it
    RbTreeDeleteNode(tree, successor);
}

/**
 * delete case when node has no children and color is black
 * **/
static void RbTreeDeleteCase4(struct RbTreeNode *node, struct RbTree *tree) {
    //brother must exist,But color is uncertain
    struct RbTreeNode *brother = RbTreeBrotherNode(node);
    if (RbNodeColorBlack(brother)) {
        struct RbTreeNode *right = RbTreeRightChild(brother);
        struct RbTreeNode *left = RbTreeLeftChild(brother);
        if (right && RbNodeColorRed(right) && RbTreeIsLeft(node)) {
            RbTreeRotateLeft3(node);
        } else if (left && RbNodeColorRed(left) && RbTreeIsLeft(node)) {
            RbTreeRotateRight4(node);
            RbTreeRotateLeft3(node);
        } else if (right && RbNodeColorRed(right) && !RbTreeIsLeft(node)) {
            RbTreeRotateLeft4(node);
            RbTreeRotateRight3(node);
        } else if (left && RbNodeColorRed(left) && !RbTreeIsLeft(node)) {
            RbTreeRotateRight3(node);
        } else {
            RbTreeSetColorRed(brother);
            RbTreeDeleteCase4(RbTreeFather(node), tree);
        }
    } else {
        //brother is red
        if (RbTreeIsLeft(node)) {
            RbTreeRotateLeft5(node);
        } else {
            RbTreeRotateRight5(node);
        }
        //situation is transformed
        RbTreeDeleteCase4(node, tree);
    }
}

/**
 * delete node  entry
 * **/
void RbTreeDeleteNode(struct RbTree *tree, struct RbTreeNode *node) {
    if (RbTreeEmpty(tree)) {
        return;
    }
    if (RbNodeNoChildren(node) && RbNodeColorRed(node)) {
        RbTreeDeleteCase1(node, tree);
    } else if (RbTreeHasOneChild(node)) {
        RbTreeDeleteCase2(node, tree);
    } else if (RbTreeHasTwoChildren(node)) {
        //in  one  iteration cycle,it is  impossible   following code run twice
        RbTreeDeleteCase3(node, tree);
    } else {
        //node is black and no children
        RbTreeDeleteCase4(node, tree);
    }
}
