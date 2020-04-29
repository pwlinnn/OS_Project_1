#define _GNU_SOURCE
#define _USE_GNU
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include "process.h"
#include "schedule.h"

#define BUFFSIZE 256
#define MAX_PROCESS 256

int cmp(const void *a, const void *b) {
    process *p = (process *)a;
    process *p1 = (process *)b;
    if (p->readyT < p1->readyT)
        return -1;
    if (p->readyT > p1->readyT)
        return 1;
    // id identifies who comes first, useful when 2 p has the same 
    // ready time.
    if (p->id < p1->id)
        return -1;
    if (p->id > p1->id)
        return 1;
    return 0;
}

int getPolicy(char policy[BUFFSIZE]) {
    if (!strcmp(policy, "FIFO")) 
        return 0;
    if (!strcmp(policy, "RR")) 
        return 1;
    if (!strcmp(policy, "SJF")) 
        return 2;
    if (!strcmp(policy, "PSJF")) 
        return 3;
}

process processes[MAX_PROCESS];
process *queue[MAX_PROCESS];

int main() {
    int process_num;
    char policy[BUFFSIZE]={};
    scanf("%s", policy);
    scanf("%d", &process_num); 
    for (int i = 0; i < process_num; i++) {
        scanf("%s%d%d", processes[i].name, &processes[i].readyT, &processes[i].execT);
        processes[i].id = i; 
    }
    // Sort the processes based on ready time.
    // If 2 processes has the same ready time, then processes the 
    // one that came first.
    qsort(processes, process_num, sizeof(process), cmp);
    schedule(getPolicy(policy), processes, process_num);
    return 0;
}
