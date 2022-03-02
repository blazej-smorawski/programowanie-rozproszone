#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct node {
    int data;
    struct node *next;
    struct node *prev;
};

struct list {
    struct node *head;
    struct node *tail;
};

struct list list = {0};

int listAdd(int data) {
	struct node *node = calloc(1, sizeof(*node));
	if (!node) {
		return -1;
	}

	node->data = data;

	if (!list.tail) {
		/*
		 * Tail is null so head is null
		 */
		list.head = node;
		list.tail = node;
		return 0;
	}

	list.tail->next = node;
	node->prev = list.tail;
	list.tail = node;
}

void listRemove(struct node *node) {
	if (node->prev) {
		node->prev->next = node->next;
	} else {
		list.head = node->next;
	}

	if (node->next) {
		node->next->prev = node->prev;
	} else {
		list.tail = node->prev;
	}

	free(node);
}

void listPrint() {
	struct node *node = list.head;
	printf("[");
	while (node) {
		printf("%d,", node->data);
		node = node->next;
	}
	printf("]\n");
}

int listFind(int data) {
	struct node *node = list.head;
	while (node) {
		if (node->data == data) {
			return 1;
		}
		node = node->next;
	}
	return 0;
}

void *threadJob(void *arg) {
	listAdd((int)arg);
	return NULL;
}

int main(void) {
	int elems[8] = {1, 632, 2179456, 211, -15, 42, 5, 2563};
	int n = 8;
	pthread_t threads[8];
	for (int i = 0; i < n; i++) {
		pthread_create(&threads[i],NULL,threadJob,(void*)elems[i]);
	}

	for (int i = 0; i < n; i++) {
		pthread_join(threads[i], NULL);
	}
	listPrint();

	int found = 0;
	for (int i = 0; i < n; i++) {
		found += listFind(elems[i]);
	}
	printf("Found %.2f%%", (double)found/n*100);
	return 0;
}
