#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "process.h"



void add_process(int policy, process *p);
void ending_handler(int policy);
int theoritical_exe(int policy);
int schedule(int policy, process* process, int process_num);




#endif
