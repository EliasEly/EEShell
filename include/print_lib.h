#ifndef __PRINT_LIB_H__
#define __PRINT_LIB_H__

#define CYAN "\033[1;36m"
#define NONE "\033[0m"


/**
 * @brief It just prints a cool ascii art. I hope I'll get a bonus point for that.
 */
void print_ascii();

/**
 * @brief It just prints the prompt line with the username.
 */
void print_prompt(char* username);

#endif