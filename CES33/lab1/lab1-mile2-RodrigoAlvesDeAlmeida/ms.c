#include <stdio.h>
#include <string.h>
#include "process.h"
#include "types.h"
#include "parser.h"

int main(){
    printf("Bem vindo ao shell.\nExemplos de inputs estão presentes no arquivo guia.md.\nPressione q para sair.\n");
    char **argv;

    while (1){
        print_ms_symbol();

        argv = read_input();

        if(argv == NULL) continue;

        if(argv[0] != NULL && strcmp(argv[0], "q") == 0) return 0;

        execute_JOB(argv);

        free_CHARPP(argv);
    }

    return 0;
}

