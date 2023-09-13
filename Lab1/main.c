/*
#include "stdio.h"
#include "unistd.h"
#include "sys/wait.h"

int main() {
    pid_t child_id;
    int status;

    child_id = fork();

    if (child_id < 0) {
        perror("Fork failed");
    } else if (child_id == 0){
        char *argv[4] = {"mkdir", "-p", "sample-dir", NULL};
        execv("/bin/mkdir", argv);
        perror("execv");
    } else {
        while ((wait(&status)) > 0);

        char *argv[3] = {"touch", "sample-dir/sample-touch.txt", NULL};
        execv("/usr/bin/touch", argv);
        perror("execv");
    }
    return 0;
}*/

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid;
    int status;

    child_pid = fork();

    if (child_pid == -1) {
        perror("Fork failed");
        return 1;
    }

    if (child_pid == 0) {
        // This code runs in the child process
        char *program = "/Applications/Figma.app/Contents/MacOS/figma"; // Specify the full path to Figma
        char *args[] = {program, NULL};
        execvp(program, args);

        // If execvp() fails, it will reach this point
        perror("Execvp failed");
        return 1;
    } else {
        // This code runs in the parent process
        printf("Parent process. PID: %d\n", getpid());
        printf("Child process. PID: %d\n", child_pid);

        // Wait for the child process to finish
        wait(&status);
    }

    return 0;
}
