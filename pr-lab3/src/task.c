//
// Created by blazej-smorawski on 06.04.2022.
//
#include <stdio.h>
#include <unistd.h>
#include "task.h"

struct task_output is_prime_task(void *arg) {
    int ret = 1;
    struct prime_task_data data = ((struct task*)arg)->prime;
    unsigned long number = data.number;
    for(long i=2;i<number;i++) {
        if(number%i==0) {
            ret = 0;
            break;
        }
    }
    printf("[%d] %lu isPrime: %d\n", getpid(), number, ret);
    struct task_output output;
    output.prime.number = number;
    output.prime.is_prime = ret;
    return output;
}