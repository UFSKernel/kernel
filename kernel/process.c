#include <process.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

process current = NULL;

pid_t used_pids[MAX_PROCESS_COUNT] = {0};
pid_t next_pid = 1;

pid_t create_pid(){
    for(int i = 0; i < MAX_PROCESS_COUNT; i++){
        pid_t pid = next_pid;
        next_pid++;

        if(next_pid >= MAX_PROCESS_COUNT){
            next_pid = 1;
        }

        if(!used_pids[pid]){
            used_pids[pid] = 1;
            return pid;
        }
    }
    return MAX_PROCESS_COUNT;
}

void free_pid(pid_t pid){
    used_pids[pid] = 0;
}
