#ifndef LAB0_TIMSORT_H
#define LAB0_TIMSORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

#define DO_MINRUN 1

typedef int (*list_cmp_func_t)(void *,
                               struct list_head *a,
                               struct list_head *b);

/**
 * pair - the struct for find_run() return value
 *
 * @head: the starting point of the run
 * @next: the starting point of the next run (maybe NULL)
 */
struct pair {
    struct list_head *head;
    struct list_head *next;
};

/**
 * timsort - an improvement for list sort, to deal with sorted sublists
 *
 * Reference:
 * https://github.com/visitorckw/linux23q1-timsort/blob/main/inplace_timsort.c
 *
 */
void timsort(void *priv, struct list_head *head, list_cmp_func_t cmp);

#endif