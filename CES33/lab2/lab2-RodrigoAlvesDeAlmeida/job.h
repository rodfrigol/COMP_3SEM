#ifndef JOB_HEADER
#define JOB_HEADER

#include <sys/types.h>

typedef struct Job
{
    int number;                 //job number for user control
    pid_t pgid;                 //pgid of processes inside job
    char **argv;                //job description
    struct Job* next;           //pointer to next element in queue
    struct Job* prev;           //pointer to previous element in queue
} JOB;

/*maintains jobs at background*/
typedef struct JobQueue
{
    struct Job* first_job;      //first element of queue
    struct Job* last_job;       //last element of queue
} JOBQUEUE;

/*frees job*/
void free_JOB(JOB* j);

/*creates job from argv and pgid and inserts it into the queue*/
void insert_JOBQUEUE(JOBQUEUE* jq, pid_t master, char** argv);

/*classical queue removal*/
void remove_JOBQUEUE(JOBQUEUE* jq, JOB* j);

/*iterates throw jobs in queue, showing user wich jobs are at background when it types jobs*/
void _jobs(JOBQUEUE* jobs);

/*searches for the job number given as argument and puts it back to foreground when user types fg [NUMBER]*/
void _fg(char* arg, JOBQUEUE* jobs);

/*builds jobs' processes and run it*/
void execute_JOB(char **argv, JOBQUEUE* jq);

#endif