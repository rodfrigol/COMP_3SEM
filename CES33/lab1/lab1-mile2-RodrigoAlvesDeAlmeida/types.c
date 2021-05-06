#include "types.h"
#include <stdio.h>
#include <stdlib.h>

void print_ms_symbol(){
    printf("\033[32;1m$ \033[0m");
}

void print_CHARPP(char** c){
    printf("%s", c[0]);
    for (int i = 1; c[i] != NULL; i++){
        printf(" %s", c[i]);
    }
    printf("\n");
}

void free_CHARPP(char** c){
    if (c == NULL) return;
    for (int i = 0; c[i] != NULL; i++){
        free(c[i]);
    }
    free(c);
}
