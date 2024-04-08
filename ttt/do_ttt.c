#include "do_ttt.h"

static int move_record[N_GRIDS];
static int move_count = 0;

static void record_move(int move)
{
    move_record[move_count++] = move;
}

static void print_moves()
{
    printf("Moves: ");
    for (int i = 0; i < move_count; i++) {
        printf("%c%d", 'A' + GET_COL(move_record[i]),
               1 + GET_ROW(move_record[i]));
        if (i < move_count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

static void clean_moves()
{
    move_count = 0;
}

static int get_input(char player)
{
    char *line = NULL;
    size_t line_length = 0;
    int parseX = 1;

    int x = -1, y = -1;
    while (x < 0 || x > (BOARD_SIZE - 1) || y < 0 || y > (BOARD_SIZE - 1)) {
        printf("%c> ", player);
        int r = getline(&line, &line_length, stdin);
        if (r == -1)
            exit(1);
        if (r < 2)
            continue;
        x = 0;
        y = 0;
        parseX = 1;
        for (int i = 0; i < (r - 1); i++) {
            if (isalpha(line[i]) && parseX) {
                x = x * 26 + (tolower(line[i]) - 'a' + 1);
                if (x > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    x = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // input does not have leading alphabets
            if (x == 0) {
                printf("Invalid operation: No leading alphabet\n");
                y = 0;
                break;
            }
            parseX = 0;
            if (isdigit(line[i])) {
                y = y * 10 + line[i] - '0';
                if (y > BOARD_SIZE) {
                    // could be any value in [BOARD_SIZE + 1, INT_MAX]
                    y = BOARD_SIZE + 1;
                    printf("Invalid operation: index exceeds board size\n");
                    break;
                }
                continue;
            }
            // any other character is invalid
            // any non-digit char during digit parsing is invalid
            // TODO: Error message could be better by separating these two cases
            printf("Invalid operation\n");
            x = y = 0;
            break;
        }
        x -= 1;
        y -= 1;
    }
    free(line);
    return GET_INDEX(y, x);
}

/* coroutine */
static LIST_HEAD(tasklist);
static void (**tasks)(void *); /* array of function pointer */
static struct arg *args;
static struct task *curr_task;
static jmp_buf sched; /* the env variable for schedule function */
static int ntasks;
static char table[N_GRIDS];

/* FIFO order */
static void task_add(struct task *task)
{
    list_add_tail(&task->list, &tasklist);
}

/* switch to the first task */
static void task_switch()
{
    if (!list_empty(&tasklist)) {
        // cppcheck-suppress nullPointer
        struct task *t = list_first_entry(&tasklist, struct task, list);
        list_del(&t->list);
        curr_task = t;
        longjmp(t->env, 1);
    }
}

/* main scheduler function */
void schedule()
{
    int i = 0;

    setjmp(sched);

    while (ntasks--) {
        struct arg arg_i = args[i];
        tasks[i++](&arg_i);
        // never reach here
    }

    task_switch();
}

void task1(void *arg)
{
    /* initialize task with arg */
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 16);
    task->table = ((struct arg *) arg)->table;
    // strncpy(task->table, ((struct arg*) arg)->table, N_GRIDS);
    task->turn = ((struct arg *) arg)->turn;
    INIT_LIST_HEAD(&task->list);

    /* AI 1: negamax */
    negamax_init();

    printf("%s: %c\n", task->task_name, task->turn);

    if (setjmp(task->env) == 0) {
        /* add task to tasklist */
        task_add(task);
        longjmp(sched, 1);
    }

    /* now it's curr_task turn to execute */
    task = curr_task;
    while (1) {
        if (setjmp(task->env) == 0) {
            /* check for win */
            char win = check_win(task->table);
            if (win == 'D') {
                draw_board(task->table);
                printf("It is a draw!\n");
                break;
            } else if (win != ' ') {
                draw_board(task->table);
                printf("%c won!\n", win);
                break;
            }

            /* make move */
            int move = negamax_predict(task->table, task->turn).move;
            if (move != -1) {
                task->table[move] = task->turn;
                record_move(move);
            }

            task_add(task);
            task_switch();
        }
        task = curr_task;
    }

    free(task);
}

void task2(void *arg)
{
    /* initialize task with arg */
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 16);
    task->table = ((struct arg *) arg)->table;
    // strncpy(task->table, ((struct arg*) arg)->table, N_GRIDS);
    task->turn = ((struct arg *) arg)->turn;
    INIT_LIST_HEAD(&task->list);

    /* AI 2: mcts */

    printf("%s: %c\n", task->task_name, task->turn);

    if (setjmp(task->env) == 0) {
        /* add task to tasklist */
        task_add(task);
        longjmp(sched, 1);
    }

    /* now it's curr_task turn to execute */
    task = curr_task;
    while (1) {
        if (setjmp(task->env) == 0) {
            /* check for win */
            char win = check_win(task->table);
            if (win == 'D') {
                draw_board(task->table);
                printf("It is a draw!\n");
                break;
            } else if (win != ' ') {
                draw_board(task->table);
                printf("%c won!\n", win);
                break;
            }

            /* make move */
            int move = mcts(task->table, task->turn);
            if (move != -1) {
                task->table[move] = task->turn;
                record_move(move);
            }

            task_add(task);
            task_switch();
        }
        task = curr_task;
    }
    free(task);
}

/* coroutine implementation */
bool ttt(bool ai_vs_ai)
{
    srand(time(NULL));
    memset(table, ' ', N_GRIDS);

    void (*register_task[])(void *) = {task1, task2};
    tasks = register_task;
    ntasks = ARR_SIZE(register_task);

    struct arg arg1 = {.table = table, .turn = 'O', .task_name = "negamax"};
    struct arg arg2 = {.table = table, .turn = 'X', .task_name = "mcts"};
    struct arg register_args[] = {arg1, arg2};
    args = register_args;

    INIT_LIST_HEAD(&tasklist);

    schedule();

    print_moves();
    clean_moves();

    return true;
}

/* naive version */
bool ttt2(bool ai_vs_ai)
{
    srand((uintptr_t) &ttt);
    // char table[N_GRIDS];
    memset(table, ' ', N_GRIDS);
    char turn = 'X';
    char ai = 'O';

    /* AI 1: negamax */
    negamax_init();
    /* AI 2: MCTS */

    while (1) {
        char win = check_win(table);
        if (win == 'D') {
            draw_board(table);
            printf("It is a draw!\n");
            break;
        } else if (win != ' ') {
            draw_board(table);
            printf("%c won!\n", win);
            break;
        }

        int move;
        if (turn == ai) {
            /* AI 1: negamax */
            move = negamax_predict(table, ai).move;
            if (move != -1) {
                table[move] = ai;
                record_move(move);
            }
        } else {
            draw_board(table);
            if (ai_vs_ai) {
                /* AI 2: MCTS */
                move = mcts(table, turn);
                if (move != -1) {
                    table[move] = turn;
                    record_move(move);
                }

            } else {
                /* human part */
                while (1) {
                    move = get_input(turn);
                    if (table[move] == ' ') {
                        break;
                    }
                    printf("Invalid operation: the position has been marked\n");
                }
                table[move] = turn;
                record_move(move);
            }
        }
        turn = turn == 'X' ? 'O' : 'X';
    }
    print_moves();
    clean_moves();

    return true;
}
