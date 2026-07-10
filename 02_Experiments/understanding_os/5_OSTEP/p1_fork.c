#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    // Printing Process Identifier
    printf("Hello World (pid:%d)\n", (int) getpid());
    int rc = fork(); // System call to provided by os to create new process
    if (rc < 0) {
        fprintf(stderr, "Fork Failed\n");
        exit(1);
    }
    else if (rc == 0) {
        printf("hello, I am child (pid:%d)\n", (int) getpid());
    } else {
        printf("hello , I am parent of %d (pid:%d)\n", rc, (int) getpid());
    }
    return 0;
}