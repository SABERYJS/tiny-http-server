//
// Created by Administrator on 2019/7/26 0026.
//

#include "sort_list.h"


struct SortedList *SortListCreate(int (*SortListSortMethod)(struct ListNode *node, void *data),
                                  int (*compareCallback)(struct ListNode *node, void *data),
                                  void (*clearCallback)(struct ListNode *node)) {
    struct SortedList *slt = malloc(sizeof(struct SortedList));
    if (!slt) {
        return NULL;
    } else {
        slt->list = createList(compareCallback, clearCallback);
        if (!slt->list) {
            free(slt);
            return NULL;
        } else {
            slt->sort_method = SortListSortMethod;
            return slt;
        }
    }
}


/**
 * add  new node to sorted list
 * **/
int SortListAppend(struct SortedList *list, void *data) {
    struct List *lst = list->list;
    struct ListNode *node = lst->header;
    struct ListNode *new_node = malloc(sizeof(struct ListNode));
    if (!new_node) {
        return -1;
    } else {
        new_node->data = data;
        if (lst->count == 0) {
            lst->header = lst->tail = new_node;
            lst->count++;
            return 1;
        } else {
            while (node) {
                if (list->sort_method(node, data) < 0) {
                    if (node == lst->header) {
                        //insert before first node
                        new_node->next = node;
                        node->prev = new_node;
                        lst->header = new_node;
                        lst->tail = node;
                        lst->count++;
                        return 1;
                    } else {
                        //encounter a node ,we do not care about where position of the node is
                        new_node->prev = node->prev;
                        new_node->next = node;
                        node->prev->next = new_node;
                        node->prev = new_node;
                        lst->count++;
                        return 1;
                    }
                }
                node = node->next;
            }
            //no node found ,so inserted as last node
            lst->tail->next = new_node;
            new_node->prev = lst->tail;
            lst->tail = new_node;
            lst->count++;
            return 1;
        }
    }
}