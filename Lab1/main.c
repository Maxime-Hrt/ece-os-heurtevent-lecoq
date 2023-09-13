#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "errno.h"

int mySystem(const char *command);

int main() {

    return 0;
}

int mySystem(const char *command){
    if (command == NULL || command[0] == '\0'){
        return -1;
    }

    int status;
    pid_t child_pid;

    child_pid = fork();

    //Error management
    if (child_pid < 0){
        perror("Fork failed");
        printf("Error Code: %d\n", errno);
    }

    if (child_pid == 0) {
        execl("bin/sh", "sh", "-c", command, NULL);
        perror("Exec failed");
        _exit(2);
    } else {
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -4;
        }
    }
}