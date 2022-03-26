//
// Created by blazej-smorawski on 26/03/2022.
//

#ifndef PR_LAB2_LIST_H
#define PR_LAB2_LIST_H

#include <pthread.h>

struct node {
    void *data;
    struct node *next;
    struct node *prev;
};

struct list {
    struct node *head;
    struct node *tail;
    pthread_mutex_t lock;
    pthread_cond_t *cond;
};

struct list* listCreate(pthread_cond_t *cond);
int listDestroy(struct list* list);
int listAdd(struct list* list, void *data);
void *listPop(struct list* list);
void listPrint(struct list *list);
int listFind(struct list *list, void* data);
int listEmpty(struct list *list);
#endif //PR_LAB2_LIST_H
