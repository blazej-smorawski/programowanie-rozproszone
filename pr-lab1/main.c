#include <stdio.h>
#include <stdlib.h>

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

int main(void) {
	int elems[8] = {1, 632, 2179456, 211, -15, 42, 5, 2563};
	for (int i = 0; i < sizeof(elems) / sizeof(elems[0]); i++) {
		listAdd(elems[i]);
	}
	listPrint();

	listRemove(list.head->next->next);
	listAdd(1);
	listPrint();

	int found = 0;
	for (int i = 0; i < sizeof(elems) / sizeof(elems[0]); i++) {
		found += listFind(elems[i]);
	}
	printf("Found %.2f%%", found/((double)sizeof(elems) / sizeof(elems[0]))*100);
	return 0;
}
