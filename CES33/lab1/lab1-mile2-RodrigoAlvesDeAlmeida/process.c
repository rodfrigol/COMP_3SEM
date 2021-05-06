#include "process.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

/*
    Let's say we have the following pipeline:
    /pipe1 | /pipe2 | ... | /pipeN

    PROCESS ancestry order from older to youngest:

    minishell -> master -> pipeN -> pipe(N-1) -> ... -> pipe1

*/

PROCESS* build_PROCESS(char **argv, int i, PROCESS* pipe_in){
    if (argv[i] == NULL)
        return NULL;
    
    PROCESS* p = malloc(sizeof(PROCESS));
    p->process_args = malloc(INPUT_MAXARGS*sizeof(char*));
    p->process_stdout = NULL;
    p->process_stdin = NULL;
    p->process_stderr = NULL;
    p->process_index = i;
    p->pipe_out = NULL;
    p->pipe_in = pipe_in;

    int k;
    for(k = 0; argv[i] != NULL && strcmp(argv[i], "|") != 0; i++){
        if (i>0 && argv[i-1] == NULL)
            break; /*prevents bugs from wrong inputs*/
        if (strcmp(argv[i], ">") == 0)
            p->process_stdout = argv[++i];
        else if (strcmp(argv[i], "<") == 0)
            p->process_stdin = argv[++i];
        else if (strcmp(argv[i], "2>") == 0)
            p->process_stderr = argv[++i];
        else p->process_args[k++] = argv[i];
    }
    p->process_args[k] = NULL;

    if (argv[i] != NULL && strcmp(argv[i], "|") == 0)
        p->pipe_out = build_PROCESS(argv, i+1, p); /*recursively builds another process when | operator comes*/
    
    return p;
}

void free_PROCESS(PROCESS* p){
    if (p == NULL) return;
    free(p->process_args); 
    free_PROCESS(p->pipe_in); 
    free(p); /*not needed to free other pointers since they point to argv elements, which will later be freed*/
}

void execute_PROCESS(PROCESS* p, char** argv, int* pipe_write){
    pid_t child_pid = fork();
    if (child_pid == 0){
        int fd;

        if (p->pipe_in != NULL && p->process_stdin != NULL){
            fprintf(stderr, "Process can't have pipe-in and stdin simultaneously.\n");
            _exit(1);
        }

        if (p->pipe_out != NULL && p->process_stdout != NULL){
            fprintf(stderr, "Process can't have pipe-out and stdout simultaneously.\n");
            _exit(1);
        }

        /*sets up STDs*/
        if (pipe_write != NULL)
            dup2(*pipe_write, STDOUT_FILENO);

        if (p->process_stdout != NULL){
            fd = open(p->process_stdout, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd == -1){
                perror(p->process_stdout);
                _exit(1);
            }
            dup2(fd, STDOUT_FILENO);
        }

        if (p->process_stdin != NULL){
            fd = open(p->process_stdin, O_RDONLY);
            if (fd == -1){
                perror(p->process_stdin);
                _exit(1);
            }
            dup2(fd, STDIN_FILENO);
        }

        if (p->process_stderr != NULL){
            fd = open(p->process_stderr, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd == -1){
                perror(p->process_stderr);
                _exit(1);
            }
            dup2(fd, STDERR_FILENO);
        }

        /*if has pipe-in, recursively creates process and waits its exec*/
        if (p->pipe_in != NULL){
            int pipefd[2];
            pipe(pipefd);

            execute_PROCESS(p->pipe_in, argv, &pipefd[1]);

            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
        }

        /*if pipe-in is over or doesn't have it, execs*/
        char *envp[] = {NULL};
        int ret = execve(argv[p->process_index], p->process_args, envp);

        if (ret == -1) {
            perror("");
            _exit(1);
        }
    }else{
        int status;
        wait(&status);
        if(status != 0) _exit(status);
    }
}

void execute_JOB(char** argv){
    PROCESS* p;

    p = build_PROCESS(argv, 0, NULL);

    if (p != NULL){
        /*points to pipeN (see header)*/
        while (p->pipe_out != NULL) p = p->pipe_out;

        /*creates master process*/
        if(fork()==0){
            execute_PROCESS(p, argv, NULL);
            _exit(0);
        }else{
            int s;
            wait(&s);
        }
        
    }

    free_PROCESS(p);
}