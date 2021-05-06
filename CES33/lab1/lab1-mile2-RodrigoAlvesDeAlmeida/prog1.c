#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(){
    char input[256];
    char c;
    int i = 0;
    while((c = getchar()) != EOF){
        input[i] = c;
        i++;
    }
    input[i] = '\0';
    fprintf(stdout, "\033[1;34mPrinting stdin content to stdout from pid %d: \033[0m \n%s", getpid(), input);
    fprintf(stderr, "\033[1;34mPrinting stdin content to stderr from pid %d: \033[0m \n%s", getpid(), input);
	return 0;
}