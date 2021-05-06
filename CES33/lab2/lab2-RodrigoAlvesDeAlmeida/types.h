#ifndef TYPES_HEADER
#define TYPES_HEADER

/*prints green $ char*/
void print_ms_symbol();

/*prints char** */
void print_CHARPP(char** c);

/*frees each string of vector*/
void free_CHARPP(char** c);

/*creates a copy of a vector of strings*/
char** copy_CHARPP(char** charpp);

#endif