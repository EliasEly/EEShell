#ifndef __SHELL_H__
#define __SHELL_H__

#define N_JOBS 64

struct shell {
    char* username;
    struct job* jobs[N_JOBS];
};

#endif