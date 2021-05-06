#ifndef PROCESS_HEADER
#define PROCESS_HEADER

/*works as doubly linked list*/
typedef struct Process
{
    char **process_args;        //vector of process args
    char *process_stdout;       //stdout filename, given by > operator
    char *process_stdin;        //stdin filename, given by < operator
    char *process_stderr;       //stderr filename, given by 2> operator
    int process_index;          //index of process name in argv
    struct Process* pipe_out;   //process pipe-out
    struct Process* pipe_in;    //process pipe-in
} PROCESS;

/*recursive function to build PROCESS data structure from argv*/
PROCESS* build_PROCESS(char **argv, int i, PROCESS* pipe_in);

/*recursive frees the whole linked list whose first element is p*/
void free_PROCESS(PROCESS* p);

/*executes the magic*/
void execute_PROCESS(PROCESS* p, char** argv, int* pipe_write);

/*uses the PROCESS functions to run the JOB (set of processes)*/
void execute_JOB(char** argv);

#endif