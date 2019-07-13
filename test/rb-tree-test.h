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


#ifndef STL_CLONE_RB_TREE_TEST_H
#define STL_CLONE_RB_TREE_TEST_H

#include "../src/algorithm/config_header.h"
#include "../src/algorithm/rb-tree.h"
#include "../src/algorithm/unit.h"

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
    srand(time(NULL));
    int j;
    for (j = 0; j < 10000; ++j) {
        struct Unit *tmp = malloc(sizeof(struct Unit));
        tmp->a = rand() % 100000;
        printf("insert value:%d\n", tmp->a);
        RbTreeInsertNode(tree, tmp);
    }
    /* int inertted[] = {1000, 500, 200, 100, 45, 78, 89, 789, 326, 520, 890, 903, 66, 98, 146, 210, 402, 682, 900, 970,
                       288, 564, 156, 998, 8, 6, 4, 18, 14, 600, 208};
     for (int i = 0; i < sizeof(inertted) / sizeof(int); ++i) {
         struct Unit *tmp = malloc(sizeof(struct Unit));
         tmp->a = inertted[i];
         RbTreeInsertNode(tree, tmp);
     }*/
    printf("nodes count :%d\n", RbTreeSize(tree));
    printf("black count is %d\n", RbTreeBlackCount(tree));
    RbTreeValidate(tree->root, RbTreeBlackCount(tree), 0);
    // RbTreeIterate(tree->root, printRbTreeNode);
    char buffer[10];
    memset(buffer, 0, 10);
    char *p = buffer;
    char c;
    int a;
    while ((c = getc(stdin))) {
        if (c >= '0' && c <= '9') {
            *(p++) = c;
        } else {
            a = atoi(buffer);
            memset(buffer, 0, 10);
            p = buffer;
            if (a > 0) {
                printf("input is %d\n", a);
                struct Unit *u = malloc(sizeof(struct Unit));
                u->a = a;
                struct RbTreeNode *node = RbTreeSearch(tree, u);
                if (node) {
#ifdef  RBTREE_LOG_OPEN
                    printf("search result:%d\n", RbTreeNodeValue(node));
#endif
                    RbTreeDeleteNode(tree, node);
                    printf("--node delete finished ,start validate tree--\n");
                    RbTreeValidate(tree->root, RbTreeBlackCount(tree), 0);
                }
            }
        }
    }
}

#endif //STL_CLONE_RB_TREE_TEST_H
