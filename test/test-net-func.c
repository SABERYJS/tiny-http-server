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


#include "../src/core/net.h"
#include <sys/select.h>
#include "../src/algorithm/rb-tree.h"
#include "../src/core/string-tool.h"
#include "../src/core/config.h"
#include "../src/core/log.h"
#include "../src/core/base_64.h"
#include "../src/core/http_media_type.h"
#include "../src/core/http_url_encode.h"
#include "../src/core/file.h"
#include "sort_list.h"
#include "../src/algorithm/unit.h"
#include "../src/algorithm/list.h"


int SortListCompare(struct ListNode *node, void *data) {
    struct Unit *ua = (struct Unit *) node->data;
    struct Unit *ub = (struct Unit *) data;
    return ua->a > ub->a ? 1 : ((ua->a == ub->a) ? 0 : -1);
}

int main(void) {
    struct SortedList *list = SortListCreate(SortListCompare, NULL, NULL);
    struct ListNode *node;
    struct Unit *u;
    u = malloc(sizeof(struct Unit));
    u->a = 10;
    SortListAppend(list, u);
    u = malloc(sizeof(struct Unit));
    u->a = 89;
    SortListAppend(list, u);
    u = malloc(sizeof(struct Unit));
    u->a = 1;
    SortListAppend(list, u);
    u = malloc(sizeof(struct Unit));
    u->a = 20;
    SortListAppend(list, u);
    u = malloc(sizeof(struct Unit));
    u->a = 100;
    SortListAppend(list, u);
    node = list->list->header;
    while (node) {
        u = (struct Unit *) node->data;
        printf("node value:%d\n", u->a);
        node = node->next;
    }
}