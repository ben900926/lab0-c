#include "q_sort.h"


/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *node;

    list_for_each (node, head)
        len++;
    return len;
}


/* Merge two sorted queues into one sorted one (for merge sort) */
int q_merge_two(void *priv,
                struct list_head *left,
                struct list_head *right,
                list_cmp_func_t cmp)
{
    if (!left || !right)
        return 0;

    if (list_empty(left) || list_empty(right)) {
        list_splice_init(right, left);
        return q_size(left);
    }

    LIST_HEAD(head);
    int count = 0;

    for (;;) {
        count++;
        // please note the difference between cmp and cmp_func in queue.c !!
        if (cmp(priv, left->next, right->next) >= 0) {
            list_move_tail(right->next, &head);
            if (list_empty(right)) {
                count += q_size(left);
                list_splice_init(&head, left);
                break;
            }
        } else {
            list_move_tail(left->next, &head);
            if (list_empty(left)) {
                count += q_size(right);
                list_splice_init(right, left);
                list_splice_init(&head, left);
                break;
            }
        }
    }

    return count;
}

/* Sort elements of queue in ascending/descending order */
void queue_sort(void *priv, struct list_head *head, list_cmp_func_t cmp)
{
    // zero or one element, or NULL
    if (!head || head->next == head->prev)
        return;

    // find middle point (two-pointer)
    struct list_head *start = head, *end = head;
    do {
        start = start->next;
        end = end->prev;
    } while (start != end && start->next != end);

    // parition (recursive)
    LIST_HEAD(new_head);
    list_cut_position(&new_head, head, start);

    queue_sort(priv, head, cmp);
    queue_sort(priv, &new_head, cmp);

    q_merge_two(priv, head, &new_head, cmp);
}