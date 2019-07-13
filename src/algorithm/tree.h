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
