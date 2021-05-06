#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

char** copy_CHARPP(char** charpp){
    int charpp_size;
    int charp_size;
    char** cpy;
    int i;

    for(charpp_size=0; charpp[charpp_size] != NULL; charpp_size++);
    cpy = malloc((charpp_size+1)*sizeof(char*));

    for(i=0; i<charpp_size; i++){
        for(charp_size=0; charpp[i][charp_size]!='\0'; charp_size++);
        cpy[i] = malloc((charp_size+1)*sizeof(char));
        strcpy(cpy[i], charpp[i]);
    }
    cpy[i] = NULL;

    return cpy;
}
