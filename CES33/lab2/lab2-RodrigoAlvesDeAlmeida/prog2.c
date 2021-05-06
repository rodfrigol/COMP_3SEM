#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv){
    fprintf(stdout, "prog was called from pid(%d) and %d args: ", getpid(), argc);
    for(int i=0; argv[i] != NULL ; i++){
        if (i!=0) printf("; ");
        printf("%s", argv[i]);
    }
    printf("\n");
    return 0;
}