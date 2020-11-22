#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

#include "job.h"
#include "builtin_cmd.h"

struct job* parse_command(char** command, char** envp){
    int mode = FOREGROUND_MODE;
    int pgid = -1;
    struct process *root = NULL, *next = NULL;

    int i = 0;
    for(; command[i] != NULL; i++);
    if(command[i-1][0] == '&'){
        mode = BACKGROUND_MODE;
        command[i-1] = NULL;
    }

    i = 0;
    while(command[i] != NULL){

        for(;command[i] != NULL; i++){
            if(command[i][0] == '|'){
                command[i] = NULL;
                i++;
                break;
            }
        }
        

        if(!root){
            root = build_process(command, envp);
            if(root == NULL)
                return NULL;
            next = root;
        } else {
            next->next = build_process(command, envp);
            if(next->next != NULL)
                next = next->next;
        }
        
        command += i;
        i = 0;
    }

    struct job* job = (struct job*)malloc(sizeof(struct job));
    job->mode = mode;
    job->pgid = pgid;
    job->root = root;
    job->status = NOT_LAUNCHED;
    return job;

}

int run_job(struct job* job, char** envp){
    struct process *cur = job->root;
    int fd[2], in_fd, out_fd;
    int status;

    /**
     * 3 cases possible:
     *  1 : this is the head of the process list, therefore it cannot have input changed excepted by a file
     *  2: the process is not the head nor the tail
     *  3: the process is the tail, then it should print on STDOUT or output if not NULL.
     */
    for(; cur != NULL; cur = cur->next){

        if(cur->type == EXTERNAL_CMD){
            //the process is the head, the input can be me modified
            if(cur == job->root){
                if (cur->input != NULL) {
                    in_fd = open(cur->input, O_RDONLY);
                    if(in_fd < 0){
                        printf("PANIC: File %s not found\n", cur->input);
                        in_fd = STDIN_FILENO;                    
                    }
                } else {
                    in_fd = STDIN_FILENO;
                } 
            }

            //run_process() cannot be launched before this point since the ouput/out_fd is not considered
            //the process is not the head nor the tail
            if(cur->next != NULL){
                pipe(fd);
                out_fd = fd[WRITE_END];
                run_process(cur, envp, in_fd, out_fd);
                //printf("DEBUG: %s with pid %d \n", cur->path, cur->pid);
                close(out_fd);
                in_fd = fd[READ_END];
            }
            
            
            //the process is the tail, the output can be modified
            if(cur->next == NULL){
                if (cur->output != NULL) {
                    out_fd = open(cur->output, cur->output_flag, cur->output_mode);
                    if(out_fd < 0){
                        printf("PANIC: File %s not found\n", cur->input);
                        out_fd = STDOUT_FILENO;
                    }
                } else {
                    out_fd = STDOUT_FILENO;
                }
                run_process(cur, envp, in_fd, out_fd);
            }
        } else {
            if(!execute_builtin(cur, envp))
                printf("PANIC: Builtin command %s not found", cur->path);
        }
    }

    job->status = RUNNING;
    
    //FIXME : should set group pid for all the process
    job->pgid = job->root->pid;

    if(job->mode == FOREGROUND_MODE){
        struct job *j = job;
        for(; j->root != NULL; j->root = j->root->next){
            status = 0;
            do {
                if(waitpid(j->root->pid, &status, 0) == -1)
                    break;
            } while(!WIFEXITED(status));
        }
        clear_job(job);
        return FOREGROUND_MODE;
    }

    return BACKGROUND_MODE;
}

void print_job_status(struct job* job){
    printf("[%d] %s", job->pgid, STRING_STATUS[job->status]);
}

void clear_job(struct job* job){
    for(; job->root != NULL; job->root = job->root->next)
        clear_process(job->root);

    free(job);
}