//
// Created by blazej-smorawski on 26/03/2022.
//
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "task.h"

struct workers {
    int in[2];
    int out[2];
    size_t tasks_done;
    size_t tasks_count;
    struct list *output_list;
};

void worker_loop(struct workers *workers) {
    struct task task;
    struct task_output output;
    while (read(workers->in[0], &task, sizeof(task))) {
        printf("Perform task = %d by %d\n", task.prime.number, getpid());
        output = task.task_fn(&task);
        write(workers->out[1], &output, sizeof(output));
    }
}

struct workers *workersCreate() {
    struct workers *workers = calloc(1, sizeof(*workers));
    long n = (sysconf(_SC_NPROCESSORS_ONLN) - 1) | 2;
    pipe(workers->in);
    pipe(workers->out);
    for (int i = 0; i < n; i++) {
        int id = fork();
        if (id > 0) {
            /*parent process*/
            printf("This is parent section [Process id: %d].\n", getpid());
        } else if (id == 0) {
            /*child process*/
            printf("Fork created [Process id: %d].\n", getpid());
            printf("Fork parent process id: %d.\n", getppid());
            worker_loop(workers);
            return NULL;
        } else {
            /*fork creation failed*/
            printf("Fork creation failed!!!\n");
            return NULL;
        }
    }
    return workers;
}

int workersMap(struct workers *workers, struct list *input, struct list *output) {
    workersFinish(workers);
    struct task *task;
    while ((task = listPop(input)) != NULL) {
        write(workers->in[1], task, sizeof(*task));
        printf("Sent to workers = %d\n", task->prime.number);
        workers->tasks_count++;
        free(task);
    }
    workers->output_list = output;
    return 0;
}

void workersFinish(struct workers *workers) {
    struct task_output *output;
    while (workers->tasks_done != workers->tasks_count) {
        output = calloc(1, sizeof(*output));
        if(read(workers->out[0], output, sizeof(*output))) {
            printf("Received output\n");
            listAdd(workers->output_list, (void *) output);
            workers->tasks_done++;
        } else {
            printf("Worker output pipe closed!!!\n");
            break;
        }
    }
}

void workersDestroy(struct workers *workers) {
    close(workers->in[0]);
    close(workers->in[1]);
    close(workers->out[0]);
    close(workers->out[1]);
    free(workers);
}