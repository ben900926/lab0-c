#ifndef LAB0_Q_SORT_H
#define LAB0_Q_SORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

#include "list.h"

typedef int (*list_cmp_func_t)(void *,
                               struct list_head *a,
                               struct list_head *b);

/* Sort elements of queue in ascending/descending order */
void queue_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);

#endif