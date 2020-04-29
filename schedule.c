#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include "process.h"
#include "schedule.h"

#define BUFFSIZE 256
#define MAX_PROCESS 256
#define HIGH 99
#define LOW 1
#define UNIT 100
#define swap(x, y) do { typeof(x) SWAP=(x); (x)=(y); (y)=SWAP; } while (0)

int next = 0, completed = 0;
process *queue[MAX_PROCESS];
void modify_info(int *still, int *now) {
    if (*still >= UNIT) {
        *still -= UNIT;
        *now += UNIT;
    }
    else {
        *now -= *still;
        *still = 0;
    }
}

void sort(int num) {
    for (int i = num; i > 0; i--)
        if (queue[i]->execT < queue[i-1]->execT)
            swap(queue[i], queue[i-1]);
        else return;
}


void add_process(int policy, process *p) {
    // FIFO and RR don't need to sort the process by execT
    if (policy == 0 || policy == 1) 
        queue[next] = p;
    else { // (P)SJF
        queue[next] = p;
        // sort queue with execT
        sort(next);
    }
    next++;
}

void ending_handler(int policy) {
    if (policy != 3 || queue[0]->execT == 0) // for RR
        for (int i = 0; i < next-1; i++)
            swap(queue[i], queue[i+1]); 
    if (queue[next-1]->execT == 0) {  // for RR
        queue[next-1] = NULL;
        next--;
        completed++;
    }
}

int theoritical_exe(int policy) {
    if (policy == 0 || policy == 2) {
        int toExe = queue[0]->execT;
        queue[0]->execT = 0;
        return toExe;
    }
    if (policy == 1) {
        int toExe;
        if (queue[0]->execT > 500) {
            toExe = 500;
            queue[0]->execT -= toExe;
        }
        else {
            toExe = queue[0]->execT;
            queue[0]->execT = 0;
        }
        return toExe;
    }
    if (policy == 3) {
        int toExe = UNIT;
        if (toExe > queue[0]->execT)
            toExe = queue[0]->execT;
        queue[0]->execT -= toExe;
        return toExe;
    }
}
int schedule(int policy, process* processes, int process_num) {
    setaffinity();
    setscheduler(getpid(), HIGH); // the scheduler should get high priority
    pid_t id2pid[MAX_PROCESS];
    int in_q = 0, now = 0;
    int toExe = 0, time_sum = 0, cur_proc_id = -1;
    for (int i = 0; i < MAX_PROCESS; i++)
        queue[i] = NULL;
    while (completed < process_num || toExe > 0) {
        while (in_q < process_num && processes[in_q].readyT <= now) {
            pid_t pid = fork();
            if (pid == 0) { // child
                setaffinity();
                setscheduler(getpid(), LOW); 
                exe(processes[in_q].name, processes[in_q].execT);
                return 0;
            }
            else { // parent
                int id = processes[in_q].id;
                id2pid[id] = pid;
                add_process(policy, &processes[in_q]);
                in_q++;
            }
        }
        if (!queue[0] && toExe == 0) { // not yet ready
            RUN_UNIT();
            now++;
        }
        else {
            if (!toExe) {
                if (cur_proc_id != -1)
                    ending_handler(policy);
                if (!queue[0])
                    continue;
                if (cur_proc_id != queue[0]->id)
                    time_sum = 0;
                cur_proc_id = queue[0]->id;
                toExe = theoritical_exe(policy);
                time_sum += toExe;
            }
	    // time for cur_proc_id to run.
            setscheduler(id2pid[cur_proc_id], HIGH);
            sched_yield();
            setscheduler(id2pid[cur_proc_id], LOW);
            modify_info(&toExe, &now);
            if (toExe == 0 && processes[cur_proc_id].execT == 0) {
                _wait(id2pid[cur_proc_id]);
                //printf("%s          %d\n", processes[cur_proc_id].name, now-processes[cur_proc_id].readyT);
            }
        }
    }
    return 0;
}
