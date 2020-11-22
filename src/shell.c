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
        for(i = 0; shell->jobs[i] != NULL && shell->jobs[i]->pgid != pid && i < N_JOBS; i++);
        
        if (WIFEXITED(status)) {
            clear_job(shell->jobs[i]);
            shell->jobs[i] = NULL;
        } else if (WIFSTOPPED(status)) {
            //update job status
        } else if (WIFCONTINUED(status)) {
            //update job status
        }
    }

}

void init_shell(char** envp, char*** envp_copy){
    // it is preferable to use sigaction rather than signal but it seems to be a way harder.
    struct sigaction sa;
    sa.sa_handler = &handler;
    sa.sa_flags = SA_RESTART;

    // when a SIGINT is caught, handler function is called.
    sigaction(SIGINT, &sa, NULL);

    // those signal are just ignored.
    signal(SIGTSTP, SIG_IGN);
    signal(SIGCONT, SIG_IGN);

    /**envp_copy = (char**) malloc((envp_s)*sizeof(char*));

    for(int i = 0; i < envp_s; i++){
        *envp_copy[i] = (char*) malloc(strlen(envp[i]) * sizeof(char));
        strcpy(*envp_copy[i], envp[i]);
    }
    *envp_copy[envp_s] = NULL;*/

    // set stdout without buffering so what is printed
	// is printed immediately on the screen.
    setvbuf(stdout, NULL, _IONBF, 0);
	setbuf(stdout, NULL);
}

void loop_shell(char** envp){
    char* line;
    char** command;
    struct job* job;
    int status;


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
                int i;
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

    int envp_s = 0;
	for (envp_s = 0; envp[envp_s] != NULL; envp_s++)
    ; 
    
    char** m_envp = malloc((envp_s + 1) * sizeof(char *));
	for (int i = 0; i < envp_s; i++) {
		m_envp[i] = malloc(strlen(envp[i]) * sizeof(char));
		m_envp[i] = strcpy(m_envp[i], envp[i]);
	}
	m_envp[envp_s] = NULL;
    
    init_shell(envp, &m_envp);
    
    loop_shell(m_envp);

    free(m_envp);
    return 0;
}