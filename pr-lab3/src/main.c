//
// Created by blazej-smorawski on 26/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "task.h"

void addRandomData(struct list *list) {
	srand(time(NULL));
	for(int i=0;i<10;i++) {
        struct task *task = calloc(1,sizeof(*task));
        task->task_fn = is_prime_task;
        task->prime.number = rand();
		listAdd(list, task);
	}
}

int main(void) {
	struct list *input_queue = listCreate(NULL);
	struct list *output_queue = listCreate(NULL);
	addRandomData(input_queue);
	struct workers *workers = workersCreate();
    if(workers == NULL) {
        /*
         * Child exits
         */
        return 0;
    }
	workersMap(workers, input_queue, output_queue);
	workersFinish(workers);

    struct task_output *task_output;
    while ((task_output = listPop(output_queue)) != NULL) {
        printf("Output: %d is prime %d\n",task_output->prime.number,task_output->prime.is_prime);
        free(task_output);
    }

    listDestroy(input_queue);
    listDestroy(output_queue);
    workersDestroy(workers);
	return 0;
}