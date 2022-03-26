//
// Created by blazej-smorawski on 26/03/2022.
//

#include <malloc.h>
#include "list.h"

struct list* listCreate(pthread_cond_t *cond) {
	struct list* list = calloc(1, sizeof(*list));
	pthread_mutex_init(&list->lock, NULL);
	list->cond = cond;
	return list;
}

int listDestroy(struct list* list) {
	if(list->head != NULL) {
		/*
		 * List is not empty!
		 */
		return -1;
	}
	pthread_mutex_destroy(&list->lock);
	list->cond = NULL;
	free(list);
	return 0;
}

int listAdd(struct list* list, void *data) {
	struct node *node = calloc(1, sizeof(*node));
	if (node == NULL) {
		return -1;
	}
	node->data = data;

	pthread_mutex_lock(&list->lock);
	if (!list->tail) {
		/*
		 * Tail is null so head is null
		 */
		list->head = node;
		list->tail = node;
		return 0;
	}
	list->tail->next = node;
	node->prev = list->tail;
	list->tail = node;

	if(list->cond != NULL) {
		pthread_cond_signal(list->cond);
	}
	pthread_mutex_unlock(&list->lock);
	return 0;
}

/*
 * If condition was specified, blocks when the list is empty
 */
struct node *listPop(struct list* list) {
	pthread_mutex_lock(&list->lock);

	if(list->tail == NULL) {
		if(list->cond == NULL) {
			/*
			 * No condition, so we just return NULL if list
			 * is empty
			 */
			return NULL;
		}
		while (list->tail == NULL) {
			/*
			 * No elements to pop, but we have a condition,
			 * so we wait
			 */
			pthread_cond_wait(list->cond, &list->lock);
		}
	}

	struct node *ret = list->tail;

	if(list->tail->prev != NULL) {
		/*
		 * Tail->prev is not NULL, so there are at
		 * least two elements in the queue
		 */
		list->tail->prev->next = NULL;
		list->tail = list->tail->prev;
	}
	else {
		/*
		 * There was only one element in the list
		 */
		list->head = NULL;
		list->tail = NULL;
	}
	/*
	 * Detach returned element from the list
	 */
	ret->prev = NULL;

	pthread_mutex_unlock(&list->lock);
	return ret;
}

void listPrint(struct list *list) {
	struct node *node = list->head;
	printf("[");
	while (node) {
		printf("%p,", node->data);
		node = node->next;
	}
	printf("]\n");
}

int listFind(struct list *list, void* data) {
	struct node *node = list->head;
	while (node) {
		if (node->data == data) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}