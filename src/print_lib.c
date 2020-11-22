#include <stdio.h>
#include "print_lib.h"

void print_ascii(){
    printf("    ____________   _____ __  __________    __ \n");
    printf("   / ____/ ____/  / ___// / / / ____/ /   / / \n"); 
    printf("  / __/ / __/     \\__ \\/ /_/ / __/ / /   / /  \n");
    printf(" / /___/ /___    ___/ / __  / /___/ /___/ /___\n");
    printf("/_____/_____/   /____/_/ /_/_____/_____/_____/\n");
}

void print_prompt(char* username){
    printf(CYAN "%s > " NONE, username);
}