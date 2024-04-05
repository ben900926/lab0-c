#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"

#include "agents/mcts.h"
#include "agents/negamax.h"
#include "agents/reinforcement_learning.h"

#include <setjmp.h>
#include "../list.h"

bool ttt(bool ai_vs_ai);

/**
 * Coroutine Settings
 *
 * ref: https://github.com/sysprog21/concurrent-programs/blob/master/coro/coro.c
 * details can check test.c
 *
 */
#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct task {
    jmp_buf env;
    struct list_head list;
    char *table;
    char turn;
    char *task_name;
};

struct arg {
    char *table;
    char turn;
    char *task_name;
};