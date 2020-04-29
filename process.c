#define _GNU_SOURCE
#define _USE_GNU
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#define UNIT 100

typedef long long ll;

void RUN_UNIT() {
    volatile unsigned long j;
    for(j = 0; j < 1000000UL; j++); 
}

ll now() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000000000ll + ts.tv_nsec;
}

void _wait(int ID) {
    int st;
    waitpid(ID, &st, 0);
}

void setaffinity() {
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(0, &mask);
    sched_setaffinity(0, sizeof(cpu_set_t), &mask);
}

void setscheduler(pid_t pid, int priority) {
    struct sched_param par;
    par.sched_priority = priority;
    sched_setscheduler(pid, SCHED_FIFO, &par);
}
void exe(char *name, int execT){
    pid_t pid = getpid();
    ll begin = now();
    for(int i = 0; i < execT; i++){
        if(i % UNIT == 0)
            if (i != 0)
                sched_yield();
        RUN_UNIT();
    }

    ll finish=now();
    ll turnAround=finish-begin;

    printf("%s ", name);
    printf("%d\n", pid);
    FILE* f=fopen("dmes", "a");
    
    fprintf(f,"[Project1] %d %lld.%09lld %lld.%09lld\n",pid,begin/1000000000ll, begin%1000000000ll,finish/1000000000ll, finish%1000000000ll);
    fclose(f);

    FILE* f1=fopen("dmes1", "a");
    fprintf(f1,"[Project1] %d %lld.%09lld %lld.%09lld\n",pid,begin/1000000000ll, begin%1000000000ll,finish/1000000000ll, finish%1000000000ll);
    fprintf(f1,"%lld.%09lld ",turnAround/1000000000ll, turnAround%1000000000ll);
    fprintf(f1,"\n");
    fclose(f1);
    return ;
}
