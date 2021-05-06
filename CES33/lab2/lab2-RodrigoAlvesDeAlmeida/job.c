#include "job.h"
#include "types.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
    Let's say we have the following pipeline:
    /pipe1 | /pipe2 | ... | /pipeN

    PROCESS ancestry order from older to youngest:

    minishell -> master -> pipeN -> pipe(N-1) -> ... -> pipe1
*/

void free_JOB(JOB* j){
    free_CHARPP(j->argv);
    free(j);
}

void insert_JOBQUEUE(JOBQUEUE* jq, pid_t master, char** argv){
    JOB* j = (JOB*)malloc(sizeof(JOB));
    j->number = (jq->last_job == NULL) ? 1 : (jq->last_job->number + 1);
    j->pgid = master;
    j->argv = copy_CHARPP(argv);
    j->next = NULL;
    j->prev = jq->last_job;

    if(jq->first_job == NULL){
        jq->first_job = jq->last_job = j;
    }else{
        jq->last_job->next = j;
        jq->last_job = j;
    }
}

void remove_JOBQUEUE(JOBQUEUE* jq, JOB* j){
    if(j == jq->first_job)
        jq->first_job = j->next;
    if(j == jq->last_job)
        jq->last_job = j->prev;
    if(j->prev != NULL)
        j->prev->next = j->next;
    if(j->next != NULL)
        j->next->prev = j->prev;
    free_JOB(j);
}

void _jobs(JOBQUEUE* jobs){
    if(jobs->last_job != NULL){
        printf("The following processes are stopped. Type fg [NUMBER] to put the process to foreground.\n");
        printf("\033[1;34mNUMBER\t\t\tPGID\t\t\tCMD\033[0m\n");
        
        for(JOB* j = jobs->first_job; j != NULL; j = j->next){
            printf("%d\t\t\t%d\t\t\t", j->number, j->pgid);
            print_CHARPP(j->argv);
        }
    }
}

void _fg(char* arg, JOBQUEUE* jobs){
    if (arg == NULL){
        printf("Error: type a process number.\n");
        return;
    }

    int num = atoi(arg);
    if (num <= 0){
        printf("Error: invalid process number.\n");
        return;
    }

    for(JOB* j = jobs->first_job; j != NULL; j = j->next){
        if(j->number == num){
            /*gives terminal control to the job that is about to be put to foreground*/
            tcsetpgrp(STDIN_FILENO, j->pgid);

            /*sends signal to all elements in process group*/
            killpg(j->pgid, SIGCONT);

            /*waits execution*/
            int status;
            waitpid(j->pgid, &status, WUNTRACED);

            /*gives terminal control back to mini-shell*/
            tcsetpgrp(STDIN_FILENO, getpid());

            if(!WIFSTOPPED(status)){
                /*if job exited ou was interrupted, remove from queue*/
                remove_JOBQUEUE(jobs, j);
                if(WIFSIGNALED(status)) printf("\033[0;31mProcess interrupted.\033[0m\n");
            }else{
                printf("\033[1;34mThe process was stopped. Type jobs to get all stopped jobs.\033[0m\n");
            }
            return;
        }
    }

    printf("Error: process number not found.\n");
}

void execute_JOB(char **argv, JOBQUEUE* jq){
    PROCESS* p = build_PROCESS(argv, 0, NULL);

    if (p != NULL){
        /*as shown in header, we need to start with last pipe element*/
        while (p->pipe_out != NULL) p = p->pipe_out;

        /*creates master process shown in header, to 'control' the job*/
        pid_t master = fork();
        if(master == 0){
            /*gives terminal control to master*/
            tcsetpgrp(STDIN_FILENO, getpid());

            /*creates new process group, where master is leader*/
            setpgrp();

            /*enables signals for master*/
            signal(SIGINT, SIG_DFL);
            signal(SIGQUIT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);
            signal(SIGTTIN, SIG_DFL);
            signal(SIGTTOU, SIG_DFL);

            execute_PROCESS(p, argv, NULL);

            _exit(0);

        }else{
            /*gives terminal control to master*/
            tcsetpgrp(STDIN_FILENO, master);

            /*waits for master exit/interrupt/stop*/
            int status;
            int pid = waitpid(-1, &status, WUNTRACED);

            /*gives terminal control back to mini-shell*/
            tcsetpgrp(STDIN_FILENO, getpid());

            if(WIFSTOPPED(status)){
                /*if stopped, sends job to background queue*/
                printf("\033[1;34mThe process was stopped. Type jobs to get all stopped jobs.\033[0m\n");
                insert_JOBQUEUE(jq, master, argv);
            }else if(WIFSIGNALED(status)){
                printf("\033[0;31mProcess interrupted.\033[0m\n");
            }
        }
    }

    free_PROCESS(p);
}