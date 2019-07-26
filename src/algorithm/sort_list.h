//
// Created by Administrator on 2019/7/26 0026.
//

#ifndef STL_CLONE_SORT_LIST_H
#define STL_CLONE_SORT_LIST_H

#include "config_header.h"
#include "list.h"

typedef int (*SortListSortMethod)(struct ListNode *node, void *data);//to sort list

struct SortedList {
    struct List *list;
    SortListSortMethod sort_method;
};

struct SortedList *SortListCreate(int (*SortListSortMethod)(struct ListNode *node, void *data),
                                  int (*compareCallback)(struct ListNode *node, void *data),
                                  void (*clearCallback)(struct ListNode *node));

int SortListAppend(struct SortedList *list, void *data);

#endif //STL_CLONE_SORT_LIST_H
