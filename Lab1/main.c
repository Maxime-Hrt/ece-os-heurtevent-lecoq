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
}
 */