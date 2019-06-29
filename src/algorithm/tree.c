//
// Created by Administrator on 2019/6/16.
//

#include "tree.h"

struct Tree *createTree(int (*compare)(struct TreeNode *, void *), void (*clear)(struct TreeNode *)) {
    struct Tree *tree = (struct Tree *) malloc(sizeof(struct Tree));
    if (!tree) {
        return NULL;
    } else {
        tree->count = 0;
        tree->compare = compare;
        tree->clear = clear;
        tree->root = NULL;
        return tree;
    }
}

struct TreeNode *insertNode(struct Tree *tree, void *data) {
    struct TreeNode *nd = malloc(sizeof(struct TreeNode));
    nd->lNode = nd->rNode = nd->pNode = NULL;
    nd->data = data;
    if (tree->count == 0) {
        tree->root = nd;
    } else {
        struct TreeNode *td = tree->root;
        while (td) {
            if (tree->compare(td, data) >= 0) {
                if (!td->lNode) {
                    nd->pNode = td;
                    td->lNode = nd;
                    break;
                } else {
                    td = td->lNode;
                }
            } else {
                if (!td->rNode) {
                    nd->pNode = td;
                    td->rNode = nd;
                    break;
                } else {
                    td = td->rNode;
                }
            }
        }
    }
    tree->count++;
    return nd;
}

void deleteNode(struct Tree *tree, struct TreeNode *root, struct TreeNode *node) {
    if (tree->count == 0 || !root) {
        return;
    } else {
        if (root == node) {
            if (tree->count == 1) {
                tree->count = 0;
                tree->root = NULL;
                return;
            } else {
                if (root == root->pNode->lNode) {
                    if (!root->rNode) {
                        if (!root->lNode) {
                            return;
                        } else {
                            root->pNode->lNode = root->lNode;
                            root->lNode->pNode = root->pNode;
                        }
                    } else {
                        root->pNode->lNode = root->rNode;
                        root->rNode->pNode = root->pNode;
                        if (root->lNode) {
                            struct TreeNode *sd = root->rNode;
                            while (sd->lNode != NULL) {
                                sd = sd->lNode;
                            }
                            sd->lNode = root->lNode;
                            sd->lNode->pNode = sd;
                        }
                    }
                } else {
                    if (!root->rNode) {
                        if (!root->lNode) {
                            return;
                        }
                        root->pNode->rNode = root->lNode;
                        root->lNode->pNode = root->pNode;
                    } else {
                        struct TreeNode *td = root->rNode;
                        root->pNode->rNode = td;
                        td->pNode = root->pNode;
                        if (!root->lNode) {
                            return;
                        }
                        struct TreeNode *sd = td;
                        while (sd->lNode != NULL) {
                            sd = sd->lNode;
                        }
                        sd->lNode = root->lNode;
                        root->lNode->pNode = sd;
                    }
                }
            }
            tree->count--;
        } else {
            if (tree->compare(root, node->data) >= 0) {
                deleteNode(tree, root->lNode, node);
            } else {
                deleteNode(tree, root->rNode, node);
            }
        }
    }
}

struct TreeNode *search(struct Tree *tree, struct TreeNode *root, void *data) {
    if (tree->count == 0) {
        return NULL;
    } else {
        int ret = tree->compare(root, data);
        if (ret == 0) {
            return root;
        } else if (ret < 0) {
            if (!root->rNode) {
                return NULL;
            } else {
                return search(tree, root->rNode, data);
            }
        } else {
            if (!root->lNode) {
                return NULL;
            } else {
                return search(tree, root->lNode, data);
            }
        }
    }
}

void clearTree(struct Tree *tree, struct TreeNode *root) {
    if (root->lNode) {
        clearTree(tree, root->lNode);
    }
    if (root->rNode) {
        clearTree(tree, root->rNode);
    }
    tree->clear(root);
}

void iterateTree(struct Tree *tree, struct TreeNode *root, void(*callback)(struct TreeNode *node)) {
    callback(root);
    if (root->lNode) {
        iterateTree(tree, root->lNode, callback);
    }
    if (root->rNode) {
        iterateTree(tree, root->rNode, callback);
    }
}
