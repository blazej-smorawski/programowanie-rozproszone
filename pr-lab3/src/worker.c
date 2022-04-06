//
// Created by bsmoraws on 4/6/2022.
//

#include <stdio.h>
#include <stdlib.h>
#include <task.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if(argc<5) {
        fprintf(stderr,"argc<5");
    }

    int in[2], out[2];
    in[0]=atoi(argv[1]);
    in[1]=atoi(argv[2]);
    out[0]=atoi(argv[3]);
    out[1]=atoi(argv[4]);

    struct task task;
    while(read(in[0], &task, sizeof(task))) {
        printf("task:{\nnumber=%d\n}\n",task.number);
    }

    return 0;
}