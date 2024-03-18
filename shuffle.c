#include "shuffle.h"

/* Return number of elements in queue */
static int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *node;

    list_for_each (node, head)
        len++;
    return len;
}

static inline void swap(struct list_head *a, struct list_head *b)
{
    if (a == b)
        return;

    //  b --> a
    struct list_head *a_prev = a->prev, *b_prev = b->prev;

    if (b != a_prev)
        list_move(b, a_prev);
    if (a != b_prev)
        list_move(a, b_prev);
}

void q_shuffle(struct list_head *head)
{
    if (!head || head->prev == head->next)
        return;

    // srand(time(NULL));

    int len = q_size(head);

    struct list_head *node = head->prev, *safe = node->prev;
    while (len > 1) {
        /* random node */
        int idx = rand() % len;

        len--;

        struct list_head *swap_node = head->next;
        while (idx--)
            swap_node = swap_node->next;

        /* swap "node" with "swap_node" */
        swap(node, swap_node);

        /* update ptr */
        node = safe;
        safe = node->prev;
    }
}