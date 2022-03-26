//
// Created by blazej-smorawski on 26/03/2022.
//
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include "workers.h"

struct workers {
    int working;
    int stopped;
    pthread_t *threads;
    size_t threads_count;
    struct list *input_list;
    struct list *output_list;
    worker_task_fn task;
    pthread_cond_t task_available;
    pthread_mutex_t lock;
    size_t waiting_for_task;
};

void *workerLoop(void *arg) {
	struct workers *workers = arg;
	while (workers->working) {
		pthread_mutex_lock(&workers->lock);
		if (workers->task == NULL) {
			workers->waiting_for_task++;
			pthread_cond_wait(&workers->task_available, &workers->lock);
			workers->waiting_for_task--;
		}
		worker_task_fn my_task = workers->task;
		pthread_mutex_unlock(&workers->lock);

		void *task_input = listPop(workers->input_list);
		if (task_input == NULL) {
			/*
			 * No task, so we wait
			 */
			usleep(10);
			continue;
		}
		listAdd(workers->output_list, my_task(task_input));
	}
	return NULL;
}

struct workers *workersCreate() {
	struct workers *workers = calloc(1, sizeof(*workers));
	workers->working = 1;
	workers->threads_count = (sysconf(_SC_NPROCESSORS_ONLN) - 1) | 2;
	workers->threads = calloc(workers->threads_count, sizeof(*workers->threads));
	pthread_mutex_init(&workers->lock, NULL);
	pthread_cond_init(&workers->task_available, NULL);

	for(int i=0;i<workers->threads_count;i++) {
		pthread_create(&workers->threads[i], NULL, workerLoop, workers);
	}

	return workers;
}

void workersFinish(struct workers *workers) {
	while (!listEmpty(workers->input_list)) {
		usleep(10);
	}

	pthread_mutex_lock(&workers->lock);
	workers->task = NULL;
	pthread_mutex_unlock(&workers->lock);

	while (workers->waiting_for_task != workers->threads_count) {
		usleep(10);
	}

}

void workersDestroy(struct workers *workers) {
	workersFinish(workers);
	workers->working=0;
	pthread_cond_broadcast(&workers->task_available);
	/*
	 * All threads will spin one more time
	 */
	for(int i=0;i<workers->threads_count;i++) {
		pthread_join(workers->threads[i], NULL);
	}
	free(workers->threads);
	pthread_mutex_destroy(&workers->lock);
	pthread_cond_destroy(&workers->task_available);
	free(workers);
}

int workersMap(struct workers *workers, struct list *input, struct list *output, worker_task_fn task) {
	while (workers->waiting_for_task != workers->threads_count) {
		usleep(10);
	}

	workers->input_list = input;
	workers->output_list = output;
	workers->task = task;
	pthread_cond_broadcast(&workers->task_available);
	return 0;
}