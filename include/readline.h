#ifndef __READLINE__H__
#define __READLINE__H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Get username. The array is allocated and must be freed 
 * at the end of shell session. 
 */
char *get_username();

/*
 * Read a line from standard input into a newly allocated 
 * array of char. The allocation is done via malloc, the array 
 * must be freed by free.
 */
char *readline(void);

/* 
 * Split the string in words, according to the simple shell grammar.
 * Returns a null-terminated array of words.
 * The array has been allocated by malloc, it must be freed by free.
 */
char **split_in_words(char *line);

/*
 * Remove the forbidden characters for an environment variable.
 * It doesn't allocate nor reduce the allocated size 
 * of the pointer.
 */
void trim(char* s);


/*
 * Read an environment variable and returns its name.
 * The returned array must be free'd.
 */
char* name_envp(char* s);

#endif