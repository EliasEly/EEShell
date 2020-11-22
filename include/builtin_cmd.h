#ifndef __BUILTIN__H__
#define __BUILTIN__H__

#include "process.h"

#define CD_CMD 1
#define PWD_CMD 2
#define PRINT_CMD 3
#define SET_CMD 4

/*
Cette fonction permet de changer de répertoire
@Args: array of strings (we don't care about the options so far)
@Return: -1 if it fails, 0 else
*/
int cd(char** words);

/*
Cette fonction permet d'afficher le chemin courant
@Args: array of strings (we don't care about the options so far)
*/
void pwd(char** words);

/*
 * Update the pwd envrionment variable.
 */
void update_pwd(char** envp);

/*
 * Loops through all the environment variable and prints it.
 */
void print(char** envp);

/*
 * Look if the environment variable (words[0]) exists in envp.
 * If it does then it updates the associate value.
 * Else it just increase the size of envp by 1 and add it at the end.
 */
void set(char** words, char** envp);

/*
 * It looks for the path of the executables in the environment
 * variable PATH. Returns null if the executable is not found.
 */
char* find_path(char** envp, char* exec);

/*
 * If p->type is a among the builtin commands then it executes it and return 1
 * else it returns 0.
 */
int execute_builtin(struct process *p, char** envp);

#endif