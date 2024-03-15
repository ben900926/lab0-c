#include "list_sort.h"


static void merge_final(void *priv,
                        list_cmp_func_t cmp,
                        struct list_head *head,
                        struct list_head *a,
                        struct list_head *b)
{
    struct list_head *tail = head;
    unsigned int count = 0;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            tail->next = a;
            a->prev = tail;
            tail = a;
            a = a->next;
            if (!a)
                break;
        } else {
            tail->next = b;
            b->prev = tail;
            tail = b;
            b = b->next;
            if (!b) {
                b = a;
                break;
            }
        }
    }

    /* Finish linking remainder of list b on to tail */
    tail->next = b;
    do {
        /*
         * If the merge is highly unbalanced (e.g. the input is
         * already sorted), this loop may run many iterations.
         * Continue callbacks to the client even though no
         * element comparison is needed, so the client's cmp()
         * routine can invoke cond_resched() periodically.
         */
        if (!++count)
            cmp(priv, b, b);
        b->prev = tail;
        tail = b;
        b = b->next;
    } while (b);

    /* And the final links to make a circular doubly-linked list */
    tail->next = head;
    head->prev = tail;
}

static struct list_head *merge(void *priv,
                               list_cmp_func_t cmp,
                               struct list_head *a,
                               struct list_head *b)
{
    struct list_head *head = NULL, **tail = &head;

    for (;;) {
        /* if equal, take 'a' -- important for sort stability */
        if (cmp(priv, a, b) <= 0) {
            *tail = a;
            tail = &a->next;
            a = a->next;
            if (!a) {
                *tail = b;
                break;
            }
        } else {
            *tail = b;
            tail = &b->next;
            b = b->next;
            if (!b) {
                *tail = a;
                break;
            }
        }
    }
    return head;
}

void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    /* pending is a prev-linked "list of lists" of sorted sublists awaiting to
     * be merged */
    struct list_head *list = head->next, *pending = NULL;
    size_t count = 0; /* Count of pending */

    /* zero or one element */
    if (list == head->prev)
        return;

    /* Convert to null-terminated singly-linked list */
    head->prev->next = NULL;

    do {
        size_t bits;
        struct list_head **tail = &pending;

        /* find the least significant bit of count */
        for (bits = count; bits & 1; bits >>= 1)
            tail = &(*tail)->prev;
        if (bits) {
            /* perform merge */
            struct list_head *a = *tail, *b = a->prev;
            a = merge(priv, cmp, a, b);

            /* install b's prev link to new merged sublist */
            a->prev = b->prev;
            *tail = a;
        }
        /* move one element from list to pending */
        list->prev = pending;
        pending = list;
        list = list->next;
        pending->next = NULL;  // needed?
        count++;

    } while (list);

    /* final merge */
    list = pending;
    pending = pending->prev;
    for (;;) {
        struct list_head *next = pending->prev;

        if (!next)
            break;
        list = merge(priv, cmp, pending, list);
        pending = next;
    }
    /* The final merge, rebuilding prev links */
    merge_final(priv, cmp, head, pending, list);
}