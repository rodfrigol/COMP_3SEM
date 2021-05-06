#ifndef PARSER_HEADER
#define PARSER_HEADER

/*max number of args from input*/
#define INPUT_MAXARGS 100

/*max size of each arg*/
#define ARG_MAXSIZE 100

/*max size of input*/
#define INPUT_MAXSIZE INPUT_MAXARGS*ARG_MAXSIZE

/*parse input into vector of strings*/
char** read_input();

#endif