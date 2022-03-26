//
// Created by blazej-smorawski on 26/03/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "workers.h"

void *isPrimeTask(void *arg) {
	int ret = 1;
	unsigned long number = (long)arg;
	for(long i=2;i<number;i++) {
		if(number%i==0) {
			ret = 0;
			break;
		}
	}
	printf("[%lu] %lu isPrime: %d\n",pthread_self(), number, ret);
	return (void*)ret;
}

void addRandomData(struct list *list) {
	srand(time(NULL));
	for(int i=0;i<100;i++) {
		listAdd(list, (void*)rand());
	}
}

int main(void) {
	struct list *input_queue = listCreate(NULL);
	struct list *output_queue = listCreate(NULL);
	addRandomData(input_queue);
	struct workers *workers = workersCreate();
	workersMap(workers, input_queue, output_queue, isPrimeTask);
	workersFinish(workers);
	printf("----====----\n");
	addRandomData(input_queue);
	workersMap(workers, input_queue, output_queue, isPrimeTask);
	workersDestroy(workers);
	return 0;
}