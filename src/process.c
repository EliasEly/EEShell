#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "process.h"
#include "builtin_cmd.h"

struct process* build_process(char** command, char** envp){
    char* path;
    char** argv;
    int output_flag = -1, output_mode = -1;
    char *output = NULL, *input = NULL;
    int argc, type = -1, i = 0;

    for(argc = 0; command[argc] != NULL; argc++);
    


    if(strcmp(command[0], "cd") == 0){
		type = CD_CMD;
	} else if (strcmp(command[0],"pwd") == 0){
		type = PWD_CMD;
	} else if (strcmp(command[0], "print") == 0){
		type = PRINT_CMD;
	} else if (strcmp(command[0], "set") == 0){
		type = SET_CMD;
	} 
    path = command[0];
    argv = command;
    if(type != -1)
        goto PACK;

    path = find_path(envp, command[0]);
    argv = command;
    
    if (path == NULL){
        printf("EE SHELL: command not found : %s\n", command[0]);
        return NULL;
    }

    type = EXTERNAL_CMD;

    while (command[i] != NULL){
        if (command[i][0] == '>') {
            output_flag = O_CREAT|O_WRONLY; 
            output = command[i+1];
            if (command[i+1] != NULL && command[i+1][0] == '>'){
                output_flag |=  O_APPEND;
                output = command[i+2];
            }
            output_mode = S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH;
            command[i] = NULL;
            break;
        } else if (command[i][0] == '<'){
            input = command[i+1];
            command[i] = NULL;
            break;
        } else {
            i++;
        }
    }
    
    argc = i;
    
    PACK:;

    struct process* p = (struct process*)malloc(sizeof(struct process));
    p->pid = -1;
    p->type = type;
    p->path = path;
    p->argc = i;
    p->argv = command;
    p->input = input;
    p->output = output;
    p->output_flag = output_flag;
    p->output_mode = output_mode;
    p->next = NULL;

    return p;
}

void run_process(struct process* p, char** envp, int in_fd, int out_fd){
    if (p == NULL){
        return;
    }

    pid_t pid;
    pid = fork();

    switch (pid) {
    case -1:
        perror("PANIC: Fork failed\n");
        exit(-1);
    case 0:
    
        if(in_fd != STDIN_FILENO){
            dup2(in_fd, STDIN_FILENO);
            close(in_fd);
        }

        if(out_fd != STDOUT_FILENO){
            dup2(out_fd, STDOUT_FILENO);
            close(out_fd);
        }

        execve(p->path, p->argv, envp);
        printf("PANIC: Couldn't execute %s\n", p->path);
        exit(-1);
        break;    
    default:
        p->pid = pid;
        break;
    }

    return;
}

void clear_process(struct process* p){
    free(p->path);
    free(p);
}
