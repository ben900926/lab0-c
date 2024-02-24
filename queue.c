#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (!head)
        return NULL;

    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    element_t *entry, *safe;
    list_for_each_entry_safe (entry, safe, l, list) {
        free(entry->value);
        free(entry);
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node_element = malloc(sizeof(element_t));
    if (!node_element)
        return false;

    node_element->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node_element->value) {
        free(node_element);
        return false;
    }

    // strcpy copies null character too
    strncpy(node_element->value, s, strlen(s) + 1);

    list_add(&node_element->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *node_element = malloc(sizeof(element_t));
    if (!node_element)
        return false;

    node_element->value = malloc((strlen(s) + 1) * sizeof(char));
    if (!node_element->value) {
        free(node_element);
        return false;
    }

    strncpy(node_element->value, s, strlen(s) + 1);

    list_add_tail(&node_element->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    // if queue is NULL or empty
    if (!head || list_empty(head))
        return NULL;

    element_t *head_element = list_first_entry(head, element_t, list);

    list_del(head->next);

    if (sp) {
        strncpy(sp, head_element->value, bufsize - 1);
        strcat(sp, "\0");
    }

    return head_element;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    // if queue is NULL or empty
    if (!head || list_empty(head))
        return NULL;

    element_t *tail_element = list_last_entry(head, element_t, list);

    list_del(head->prev);

    if (sp) {
        strncpy(sp, tail_element->value, bufsize - 1);
        strcat(sp, "\0");
    }

    return tail_element;
}

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

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    // two-pointer approach
    struct list_head *start = head, *end = head;

    do {
        end = end->prev;
        if (start == end)
            break;
        start = start->next;
    } while (start != end);

    // delete the mid node
    list_del(start);

    element_t *mid_element = list_entry(start, element_t, list);
    free(mid_element->value);
    free(mid_element);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
