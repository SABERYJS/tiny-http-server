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

#ifndef STL_CLONE_STACK_TEST_H
#define STL_CLONE_STACK_TEST_H

#include "../src/algorithm/stack.h"
#include "../src/algorithm/unit.h"

void testStack() {
    struct Stack *s = StackCreate();
    srand(time(NULL));
    int i;
    for (i = 0; i < 10; i++) {
        struct Unit *u = malloc(sizeof(struct Unit));
        u->a = rand();
        printf("push value:%d\n", u->a);
        StackPush(s, u);
    }

    printf("%d\n", STackSize(s));
    printf("top:%d\n", ((struct Unit *) StackTop(s))->a);
    StackPop(s);
    printf("top:%d\n", ((struct Unit *) StackTop(s))->a);
    struct Unit *p;
    while ((p = (struct Unit *) StackNext(s))) {
        printf("value:%d\n", p->a);
    }
    printf("%d\n", STackSize(s));
}

#endif //STL_CLONE_STACK_TEST_H
