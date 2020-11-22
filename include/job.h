#ifndef __JOB_H__
#define __JOB_H__

#include "process.h"

#define FOREGROUND_MODE 0
#define BACKGROUND_MODE 1
#define PIPELINE_MODE 2

#define RUNNING 0
#define DONE 1
#define TERMINATED 2
#define INTERRUPTED 3
#define NOT_LAUNCHED 4

#define READ_END 0
#define WRITE_END 1

static const char* STRING_STATUS[] = {
    "Running",
    "Done",
    "Terminated",
    "Interrupted",
    "Not Launched"
};

struct job {
    int pgid;
    int mode;
    int status;
    struct process* root;
};


struct job* parse_command(char** command, char** envp);

int run_job(struct job* job, char** envp);

void print_job_id(struct job* job);

void clear_job(struct job* job);

#endif