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
#ifdef USE_RL
#include "agents/reinforcement_learning.h"
#elif defined(USE_MCTS)
#include "agents/mcts.h"
#else
#include "agents/negamax.h"
#endif

bool do_ttt();