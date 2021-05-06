#include "parser.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

char** read_input(){
    char input [INPUT_MAXSIZE];
    char** argv = malloc(INPUT_MAXARGS*sizeof(char*));
    
    fgets(input, INPUT_MAXSIZE, stdin);

    int j = 0;
    int k = 0;
    int quoted = 0;
    argv[k] = malloc(ARG_MAXSIZE*sizeof(char));

    for (int i = 0; input[i] != '\0'; i++){
        if (input[i] == '"'){
            quoted = !quoted;
        }else if (!isspace(input[i]) || quoted){
            argv[k][j++] = input[i];
        }else if(j != 0){
            argv[k++][j] = '\0';
            argv[k] = malloc(ARG_MAXSIZE*sizeof(char));
            j = 0;
        }
    }

    /*argv end condition: NULL string*/
    free(argv[k]);
    argv[k] = NULL;

    if (quoted){
        free_CHARPP(argv);
        fprintf(stderr, "Erro na leitura da entrada: aspas não correspondentes.\n");
        return NULL;
    }
    
    return argv;
}