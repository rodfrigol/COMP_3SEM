#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INPUT_MAXSIZE 256
#define INPUT_MAXARGS 100
#define ARG_MAXSIZE 100

struct Process
{
    char **process_args;
    char *process_stdout;
    char *process_stdin;
    char *process_stderr;
    int process_index;
    struct Process* pipe_out;
    struct Process* pipe_in;
};


void print_ms_symbol(){
    printf("\033[32;1m$ \033[0m");
}

void print_charpp(char** c){
    for (int i = 0; c[i] != NULL; i++){
        printf("%s\n", c[i]);
    }
}

void free_charpp(char** c){
    if (c == NULL) return;
    for (int i = 0; c[i] != NULL; i++){
        free(c[i]);
    }
    free(c);
}

/// realiza o parsing do comando escrito
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

    free(argv[k]);
    argv[k] = NULL;

    if (quoted){
        free_charpp(argv);
        return NULL;
    }
    
    return argv;
}

/// Função recursiva que constrói uma estrutura de dados com as informações de cada processo e seus pipes
struct Process* build_processes(char **argv, int i, struct Process* pipe_in){
    if (argv[i] == NULL)
        return NULL;
    
    struct Process* p = malloc(sizeof(struct Process));
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
            break; /*input errado*/
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
        p->pipe_out = build_processes(argv, i+1, p);
    
    return p;
}

void free_process(struct Process* p){
    if (p == NULL) return;
    free(p->process_args); 
    free_process(p->pipe_in); free(p); /* não é necessario dar free nos outros ponteiros pois eles apontam para elementos do argv, os quais serao liberados depois*/
}

int execute_process(struct Process* p, char** argv, int* pipe_write){
    pid_t child_pid = fork();
    if (child_pid == 0){
        int fd;

        if (p->pipe_in != NULL && p->process_stdin != NULL) return 2;

        if (p->pipe_out != NULL && p->process_stdout != NULL) return 3;

        if (pipe_write != NULL)
            dup2 (*pipe_write, STDOUT_FILENO);

        if (p->process_stdout != NULL){
            fd = open(p->process_stdout, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd == -1){
                perror(p->process_stdout);
                return 1;
            }
            dup2 (fd, STDOUT_FILENO);
        }

        if (p->process_stdin != NULL){
            fd = open(p->process_stdin, O_RDONLY);
            if (fd == -1){
                perror(p->process_stdin);
                return 1;
            }
            dup2 (fd, STDIN_FILENO);
        }

        if (p->process_stderr != NULL){
            fd = open(p->process_stderr, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
            if (fd == -1){
                perror(p->process_stderr);
                return 1;
            }
            dup2 (fd, STDERR_FILENO);
        }

        if (p->pipe_in != NULL){
            int pipefd[2];
            pipe(pipefd);

            int ret = execute_process(p->pipe_in, argv, &pipefd[1]);
            if (ret != 0) return ret;

            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
        }

        char *envp[] = {NULL};
        int ret = execve(argv[p->process_index], p->process_args, envp);

        if (ret == -1) {
            perror("");
            return 1;
        }
    }else{
        int status;
        wait(&status);
        return 0;
    }
}

int main(){
    printf("Bem vindo ao shell.\nExemplos de inputs estão presentes no arquivo guia.md.\nPressione q para sair.\n");
    char **argv;
    struct Process* p;

    while (1){
        print_ms_symbol();

        argv = read_input();

        if(argv == NULL){
            fprintf(stderr, "Erro na leitura da entrada: aspas não correspondentes.\n");
            continue;
        }

        if(argv[0] != NULL && strcmp(argv[0], "q") == 0) return 0;

        p = build_processes(argv, 0, NULL);

        if (p != NULL){
            while (p->pipe_out != NULL) p = p->pipe_out;

            int ret = execute_process(p, argv, NULL);

            /// acaba com processo se ele não for o pai
            if (ret != 0) {
                if (ret == 2) fprintf(stderr, "Process can't have pipe-in and stdin simultaneously.\n");
                if (ret == 3) fprintf(stderr, "Process can't have pipe-out and stdout simultaneously.\n");
                return ret;
            }

        }

        free_process(p);
        free_charpp(argv);
    }

    return 0;
}

