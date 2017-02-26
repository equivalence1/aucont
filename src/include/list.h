#ifndef __LIST_H__
#define __LIST_H__

#include "common.h"

struct list_head {
    struct list_head *next, *prev;
};
typedef struct list_head list_head_t;

static inline
void INIT_LIST_HEAD(list_head_t *head)
{
    head->prev = head;
    head->next = head;
}

static inline
void list_add_tail(list_head_t *head, list_head_t *new_el)
{
    new_el->next = head;
    new_el->prev = head->prev;
    head->prev->next = new_el;
    head->prev = new_el;
}

static inline
int list_length(list_head_t *head)
{
    int size = 0;
    list_head_t *cur = head->next;
    while (cur != head) {
        size += 1;
        cur = cur->next;
    }
    return size;
}

#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

#endif // __LIST_H__
