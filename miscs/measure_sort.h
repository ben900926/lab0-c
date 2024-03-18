#ifndef LAB0_MEASURESORT_H
#define LAB0_MEASURESORT_H


#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "list.h"
#include "list_sort.h"
#include "q_sort.h"
#include "timsort.h"

#define SAMPLES 10
#define CHAR_LEN 10


typedef struct {
    char *value;
    int seq;
    struct list_head list;
} element_t;

typedef void (*test_func_t)(void *priv,
                            struct list_head *head,
                            list_cmp_func_t cmp);

typedef struct {
    char *name;
    test_func_t impl;
} test_t;

static const char charset[] = "abcdefghijklmnopqrstuvwxyz";


#endif