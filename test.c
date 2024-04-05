#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

#define ARR_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct task {
    jmp_buf env;
    struct list_head list;
    int n; /* how many time yield control */
    int i; /* iterator */
    char task_name[16];
};

struct arg {
    int n;
    int i;
    char *task_name;
};

static LIST_HEAD(tasklist);
static void (**tasks)(void *); /* array of function pointer */
static struct arg *args;
static struct task *curr_task;
static jmp_buf sched; /* the env variable for schedule function */
static int ntasks;

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

void schedule(void)
{
    static int i = 0;  // what for ?

    setjmp(sched);

    while (ntasks-- > 0) {
        struct arg arg_i = args[i];
        tasks[i++](&arg_i);
        printf("Never reached\n");  // ?
    }

    task_switch();
}

static long long fib_sequence(long long k)
{
    /* FIXME: use clz/ctz and fast algorithms to speed up */
    long long f[k + 2];

    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= k; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    return f[k];
}

/* A task yields control n times */

void task0(void *arg)
{
    /* initialize task with arg */
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 16);
    task->n = ((struct arg *) arg)->n;
    task->i = ((struct arg *) arg)->i;
    INIT_LIST_HEAD(&task->list);

    printf("%s: n = %d\n", task->task_name, task->n);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }

    task = curr_task;

    for (; task->i < task->n; task->i += 2) {
        if (setjmp(task->env) == 0) {
            long long res = fib_sequence(task->i);
            printf("%s fib(%d) = %lld\n", task->task_name, task->i, res);
            task_add(task);
            task_switch();
        }
        task = curr_task;
        printf("%s: resume\n", task->task_name);
    }

    printf("%s: complete\n", task->task_name);
    longjmp(sched, 1);
}

/* A task yields control n times */

void task1(void *arg)
{
    /* initialize task with arg */
    struct task *task = malloc(sizeof(struct task));
    strncpy(task->task_name, ((struct arg *) arg)->task_name, 16);
    task->n = ((struct arg *) arg)->n;
    task->i = ((struct arg *) arg)->i;
    INIT_LIST_HEAD(&task->list);

    printf("%s: n = %d\n", task->task_name, task->n);

    if (setjmp(task->env) == 0) {
        task_add(task);
        longjmp(sched, 1);
    }

    task = curr_task;

    for (; task->i < task->n; task->i += 2) {
        if (setjmp(task->env) == 0) {
            printf("%s .(%d)\n", task->task_name, task->i);
            task_add(task);
            task_switch();
        }
        task = curr_task;
        printf("%s: resume\n", task->task_name);
    }

    printf("%s: complete\n", task->task_name);
    longjmp(sched, 1);
}

int main(void)
{
    void (*register_task[])(void *) = {task0, task0, task1};
    tasks = register_task;
    ntasks = ARR_SIZE(register_task);

    struct arg arg0 = {.i = 0, .n = 70, .task_name = "Task 0"};
    struct arg arg1 = {.i = 1, .n = 70, .task_name = "Task 1"};
    struct arg arg2 = {.i = 0, .n = 70, .task_name = "Task 2"};
    struct arg register_arg[] = {arg0, arg1, arg2};
    args = register_arg;

    schedule();

    return 0;
}