#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "builtin_cmd.h"
#include "readline.h"

int cd(char** words){
	return chdir(words[1]);
}

void pwd(char** words){
	char cwd[1024];
	getcwd(cwd, sizeof(cwd));
	printf("%s\n", cwd);
}

void update_pwd(char** envp){
	int oldpwd_idx;
	for(oldpwd_idx = 0; envp[oldpwd_idx] != NULL; oldpwd_idx++)
		if (strstr(envp[oldpwd_idx], "OLDPWD") != NULL)
			  break;


	for(int i = 0; envp[i] != NULL; i++){
		if (strcmp(name_envp(envp[i]), "PWD") == 0){
			envp[oldpwd_idx] = realloc(envp[oldpwd_idx], 7 + strlen(envp[i] + 4));
			strcpy(envp[oldpwd_idx], "OLDPWD=");
			strcat(envp[oldpwd_idx], envp[i]+ 4);
			break;
		}
	}

	char wd[124];
	getcwd(wd, sizeof(wd));
	char* words[3];
	words[0] = "PWD";
	words[1] = wd;
	words[2] = NULL;
	set(words, envp);
}

void print(char** envp){
	for(int i = 0; envp[i]!=NULL; i++)
		printf("%s\n", envp[i]);
}

void set(char** words, char** envp){
	int i = 0;
	int flag = 1;

	for(i = 0 ; words[i] != NULL; i++);

	if (i != 2){
		printf("PANIC: Can't set !\n");
		return;
	}

	trim(words[1]);

	for(i = 0; envp[i] != NULL; i++){

		char* name = name_envp(envp[i]);
		flag = strcmp(words[0], name);

		if (flag == 0){
		envp[i] = realloc(envp[i], 1 + strlen(name) + strlen(words[1]));
		strcpy(envp[i], name);
		strcat(envp[i], "=");
		strcat(envp[i], words[1]);
		free(name);
		return;
		}
		free(name);
	}

	envp = realloc(envp, (i+1) * sizeof(*envp));
	if(envp == NULL){
		perror("PANIC: Something went wrong with realloc (builtin_cmd.c:63)\n");
		exit(0);
	}

	envp[i] = malloc(strlen(words[1]) * sizeof(char));
	if(envp[i] == NULL){
		perror("PANIC: Something went wrong with malloc (builtin_cmd.c:69)\n");
		exit(0);
	}

	strcpy(envp[i], words[0]);
	strcat(envp[i], "=");
	strcat(envp[i], words[1]);
	envp[i+1] =  NULL;
}

char* find_path(char** envp, char* exec){
	char* path = NULL;

	for(int i = 0; envp[i] != NULL; i++){
		if (strcmp(name_envp(envp[i]), "PATH") == 0){
		path = envp[i]+5;
		break;
		}
	}

	if (path == NULL){
		printf("PANIC: Couldn't find PATH env variable !\n");
		return NULL;
	}

	char* bin_path = malloc(128 * sizeof(char));
	int i = 0;
	while(*path != '\0'){
		while(path[i] != '\0' && path[i] != ':')
			i++;

		for(int l = 0; l < i; l++)
			bin_path[l] = path[l];
			
		bin_path[i] = '/';
		bin_path[i+1] = '\0';

		strcat(bin_path, exec);

		// Flag F_OK just check if the file exists.
		// X_OK make sure you have the rights to run it.
		if (access (bin_path, F_OK|X_OK) != -1)
		return bin_path;

		path += i + 1;
		i = 0;
	}
	free(bin_path);
	return NULL;
}

int execute_builtin(struct process *p, char** envp){

	switch(p->type){
		case CD_CMD:
			if (cd(p->argv) == 0)
				update_pwd(envp);
			break;
		case PWD_CMD:
			pwd(p->argv);
			break;
		case PRINT_CMD:
			print(envp);
			break;
		case SET_CMD:
			set(p->argv + 1, envp);
			break;
		default:
			return 0;
	}
	
	return 1;
}