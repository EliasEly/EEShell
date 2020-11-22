#ifndef __PROCESS_H__
#define __PROCESS_H__

// for the builtin command see builtin_cmd.h
#define EXTERNAL_CMD 0


struct process {
    int pid;
    int type;
    char* path;
    int argc;
    char** argv;
    char* input;
    char* output;
    int output_flag;
    int output_mode;
    struct process *next;
};

struct process* build_process(char** command, char** envp);

void run_process(struct process* p, char** envp, int in_fd, int out_fd);

void clear_process(struct process* p);


#endif