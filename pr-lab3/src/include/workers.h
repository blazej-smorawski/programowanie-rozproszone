//
// Created by blazej-smorawski on 26/03/2022.
//

#ifndef PR_LAB2_WORKER_H
#define PR_LAB2_WORKER_H
#include "list.h"

typedef void* (*worker_task_fn)(void *arg);
struct workers;

struct workers *workersCreate();
void workersDestroy(struct workers *workers);
	int workersMap(struct workers *workers, struct list *input, struct list *output, worker_task_fn task);
void workersFinish(struct workers *workers);

#endif //PR_LAB2_WORKER_H
