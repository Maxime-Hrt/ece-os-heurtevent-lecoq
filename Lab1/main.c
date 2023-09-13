#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* QUESTION 2 and 3
int main() {
    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This code is executed by the child process
        printf("PID value: %d\n", pid);
        printf("I'm the child (PID: %d, Parent PID: %d)\n", getpid(), getppid());
    } else {
        // This code is executed by the parent process
        printf("PID value: %d\n", pid);
        sleep(3);
        printf("I'm the parent (PID: %d, Child PID: %d)\n", getpid(), pid);
    }
    return 0;
}
*/

/* QUESTION 4
int main() {
    int i = 5;
    if (fork() == 0) {
        i++;
        printf("Child value: %d\n", i);
    } else {
        sleep(3);
        printf("Parent value: %d\n", i);
    }
    return 0;
}
*/

int main() {
    printf("1st-level parent process. PID: %d\n", getpid());

    pid_t child1_pid = fork(); // Create the first child

    if (child1_pid == 0) {
        printf("1st-level child process 1. PID: %d, Parent PID: %d\n", getpid(), getppid());

        pid_t grandchild_pid = fork(); // Create a child for the first child

        if (grandchild_pid == 0) {
            printf("2nd-level grandchild process. PID: %d, Parent PID: %d\n", getpid(), getppid());
        }
        return 0; // Exit the first child
    }

    pid_t child2_pid = fork(); // Create the second child

    if (child2_pid == 0) {
        printf("1st-level child process 2. PID: %d, Parent PID: %d\n", getpid(), getppid());
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    return 0; // The main parent process exits
}