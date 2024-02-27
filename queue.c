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

    list_del_init(&head_element->list);

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

    list_del_init(&tail_element->list);

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
        start = start->next;
    } while (start != end && end->next != start);

    // unlink the mid node
    list_del_init(start);

    // remove the mid node
    element_t *mid_element = list_entry(start, element_t, list);
    free(mid_element->value);
    free(mid_element);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    // for each list element
    struct list_head *cur = head->next, *del = NULL;

    while (cur != head) {
        bool remove_cur;
        // remove all node that has the same string as cur
        del = cur->next;
        remove_cur = false;

        while (del != head &&
               strcmp(list_entry(del, element_t, list)->value,
                      list_entry(cur, element_t, list)->value) == 0) {
            remove_cur = true;
            list_del_init(del);
            free(list_entry(del, element_t, list)->value);
            free(list_entry(del, element_t, list));
            del = cur->next;
        }

        // assign next first so it can find next node before unlink
        cur = cur->next;
        if (remove_cur) {
            del = cur->prev;
            list_del_init(del);
            free(list_entry(del, element_t, list)->value);
            free(list_entry(del, element_t, list));
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    // traverse through list, put to head in order... so previously last element
    // be the first
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;

    int count = k - 1;
    struct list_head *node, *cut, *next_head = head;
    LIST_HEAD(new_head);

    // cut off and reverse each k-element list
    // put the reversed sublist to the head, after that the head is changed to
    // the end of inserted sublist
    list_for_each_safe (node, cut, head) {
        if (count--)
            continue;

        count = k - 1;
        list_cut_position(&new_head, next_head, node);
        q_reverse(&new_head);
        list_splice_init(&new_head, next_head);
        next_head = cut->prev;
    }
}

/* Compare value in list_head entry */
/* return true (> 0) if left is sorted after right; false (<= 0) if left before
 * right */
bool cmp_func(struct list_head *left, struct list_head *right, bool descend)
{
    char *left_value = list_first_entry(left, element_t, list)->value;
    char *right_value = list_first_entry(right, element_t, list)->value;

    // XNOR logic
    return ((strcmp(left_value, right_value) > 0) == descend);
}

/* Merge two sorted queues into one sorted one (for merge sort) */
int q_merge_two(struct list_head *left, struct list_head *right, bool descend)
{
    if (!left || !right)
        return 0;

    LIST_HEAD(head);
    while (!list_empty(left) && !list_empty(right)) {
    }

    return 0;
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
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

    q_sort(head, descend);
    q_sort(&new_head, descend);

    q_merge_two(head, &new_head, descend);
}

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
