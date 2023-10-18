#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void increment(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg++;
    *var = reg;
}

void decrement(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg--;
    *var = reg;
}

int main() {
    key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *i = (int*) shmat(shmid, (void*)0, 0);
    *i = 65;

    pid_t pid = fork();

    if (pid == 0) {  // Child process
        increment(i);
        shmdt(i);
        return 0;
    } else if (pid > 0) {  // Parent process
        decrement(i);
        wait(NULL);  // Wait for child process to finish
        printf("Final value of i: %d\n", *i);  // Could print 64, 65, or 66 depending on the order of operations
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        printf("Fork failed!\n");
        return 1;
    }

    return 0;
}
