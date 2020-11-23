#include <stdio.h>
#include <stdlib.h>
#include <signal.h> 
#include <unistd.h>
#include <sys/wait.h>

#include "shell.h"
#include "job.h"
#include "readline.h"
#include "print_lib.h"

struct shell* shell;


/* an interruption may occur at any moment, therefore printf() can not be called
 * nor any non thread-safe function.
 */
void handler(int signal){
    write(STDOUT_FILENO, "\n", 1);
}

void check_zombie(){
    int status = 0;
    pid_t pid;
    int i;

    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        
        // looking for the process
        struct process* p = NULL;
        for(i = 0; shell->jobs[i] != NULL && i < N_JOBS; i++){
            p = shell->jobs[i]->root;
            for(; p != NULL; p = p->next){
                if (p->pid == pid)
                        goto PROCESS_FOUND;
            }
        }

        PROCESS_FOUND:
        // if exited then set it pid to -1
        if (WIFEXITED(status)) {
            p->pid = -1;
            int flag = shell->jobs[i]->root->pid;
            for(p = shell->jobs[i]->root; p != NULL; p = p->next){
                if(p->pid != -1)
                    flag = p->pid;
            }

            //if all the pid of the job are equal to -1, it means the job is terminated and be removed
            if (flag == -1){
                clear_job(shell->jobs[i]);
                shell->jobs[i] = NULL;
            }

        } else if (WIFSTOPPED(status))
            shell->jobs[i]->status = INTERRUPTED;
        
    }

}

char** init_shell(char** envp){
    // it is preferable to use sigaction rather than signal but it seems to be a way harder.
    struct sigaction sa;
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;

    // when a SIGINT is caught, handler function is called.
    sigaction(SIGINT, &sa, NULL);

    // those signal are just ignored.
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCONT, SIG_IGN);

    int envp_s = 0;
	for (envp_s = 0; envp[envp_s] != NULL; envp_s++)
    ; 
    
    char** m_envp = malloc((envp_s + 1) * sizeof(char *));
	for (int i = 0; i < envp_s; i++) {
		m_envp[i] = malloc(strlen(envp[i]) * sizeof(char));
		m_envp[i] = strcpy(m_envp[i], envp[i]);
	}

	m_envp[envp_s] = NULL;

    // set stdout without buffering so what is printed
	// is printed immediately on the screen.
    setvbuf(stdout, NULL, _IONBF, 0);
	setbuf(stdout, NULL);

    return m_envp;
}

void loop_shell(char** envp){
    char* line;
    char** command;
    struct job* job;
    int status, i;


    for(;;) {
        do {
            check_zombie();
            print_prompt(shell->username);
            line = readline();
            command = split_in_words(line);
        } while (command[0] == NULL);

        status = -1;
        job = parse_command(command, envp);
        if (job != NULL){
            status = run_job(job, envp);
            if (status == BACKGROUND_MODE){
                for(i = 0; shell->jobs[i] != NULL && i < N_JOBS; i++)
                ;
                    shell->jobs[i] = job;
            }
        }

        free(line);
        free(command);
    }
}


int main(int argc, char** argv, char** envp){

    print_ascii();
    shell = malloc(sizeof(struct shell));
    shell->username = get_username();

    char** m_envp = init_shell(envp);
    
    loop_shell(m_envp);

    free(m_envp);
    return 0;
}