#ifndef LAB0_LISTSORT_H
#define LAB0_LISTSORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "list.h"

typedef int (*list_cmp_func_t)(void *, struct list_head *, struct list_head *);



/**
 * list_sort - linux kernel style merge sort
 *
 *
 * Reference:
 * https://github.com/torvalds/linux/blob/master/lib/list_sort.c
 */
void list_sort(void *priv, struct list_head *head, list_cmp_func_t cmp);


#endif