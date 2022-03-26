//
// Created by blazej-smorawski on 26/03/2022.
//
#include <unistd.h>
#include <pthread.h>

struct workers {
	pthread_t *threads;
	size_t threads_count;
};

struct workers *workersCreate() {
	struct workers *workers = calloc(1,sizeof(*workers));
	long number_of_processors = sysconf(_SC_NPROCESSORS_ONLN);
}