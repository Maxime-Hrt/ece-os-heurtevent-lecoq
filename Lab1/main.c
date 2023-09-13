#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main() {
    int status;
    int i = 5;
    if (fork() == 0){
        execlp("echo", "echo", "Hello World", NULL);
        perror("Exec failed");

        printf("%c\n", i); //is this line executed? why?
    } else {
        printf("Process ID: %d\n", getpid());

        wait(&status);
    }
    return 0;
}