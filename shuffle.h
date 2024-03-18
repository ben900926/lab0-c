#ifndef LAB0_SHUFFLE_H
#define LAB0_SHUFFLE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"

/**
 * list_for_each_prev_safe - same as list_for_each_safe but iterate with prev
 * links
 * @node: list_head pointer used as iterator
 * @safe: list_head pointer used to store info for next entry in list
 * @head: pointer to the head of the list
 *
 * The current node (iterator) is allowed to be removed from the list. Any
 * other modifications to the the list will cause undefined behavior.
 */
#define list_for_each_prev_safe(node, safe, head)                \
    for (node = (head)->prev, safe = node->prev; node != (head); \
         node = safe, safe = node->prev)

/**
 * q_shuffle() - Shuffle the queue using Fisher-Yates Shuffle
 * @head: header of queue
 */
void q_shuffle(struct list_head *head);

#endif