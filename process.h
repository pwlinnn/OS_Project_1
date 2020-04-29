#ifndef _PROCESS_H_
#define _PROCESS_H_

#define BUFFSIZE 256
#include <sys/types.h>

typedef long long ll;
typedef struct process {

char name[BUFFSIZE];
int id;
int readyT;
int execT;
} process;
void RUN_UNIT();
ll now();
void _wait(int ID);
void exe(char *name, int execT);

void setaffinity();
void setscheduler(pid_t pid, int priority);
 
// add process to the queue
void add_process(int policy, process *P);

#endif
