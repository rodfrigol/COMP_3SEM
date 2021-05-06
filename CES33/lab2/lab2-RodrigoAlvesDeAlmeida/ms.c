#include "job.h"
#include "parser.h"
#include "process.h"
#include "types.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(){
    char **argv;
    JOBQUEUE jobs = {NULL, NULL};
    
    /* Disables all signals sent to the minishell: now it cannot be controlled by external sources */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    
    printf("Bem vindo ao shell.\nExemplos de inputs estão presentes no arquivo guia.md.\nPressione q para sair.\n");

    while (1){
        print_ms_symbol();

        argv = read_input();

        if(argv == NULL) continue;

        if(argv[0] != NULL && strcmp(argv[0], "q") == 0) return 0;

        if(argv[0] != NULL && strcmp(argv[0], "jobs") == 0) {_jobs(&jobs); continue;}

        if(argv[0] != NULL && strcmp(argv[0], "fg") == 0) {_fg(argv[1], &jobs); continue;}

        execute_JOB(argv, &jobs);

        free_CHARPP(argv);
    }

    return 0;
}

