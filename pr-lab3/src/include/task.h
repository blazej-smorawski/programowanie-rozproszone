//
// Created by bsmoraws on 4/6/2022.
//

#ifndef PR_LAB2_TASK_H
#define PR_LAB2_TASK_H

#include "workers.h"

struct prime_task_data {
    int number;
};

struct task {
    worker_task_fn task_fn;
    union {
        struct prime_task_data prime;
    };
};

struct prime_task_output {
    int number;
    int is_prime;
};

struct task_output {
    union {
        struct prime_task_output prime;
    };
};

struct task_output is_prime_task(void *arg);

#endif //PR_LAB2_TASK_H
