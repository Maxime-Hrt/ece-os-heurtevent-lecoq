#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void increment(int *var) {
    (*var)++;
}

void decrement(int *var) {
    (*var)--;
}

int main() {
    key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *i = (int*) shmat(shmid, (void*)0, 0);
    *i = 65;

    pid_t pid = fork();

    if (pid == 0) {
        increment(i);
        shmdt(i);
        return 0;
    } else if (pid > 0) {
        decrement(i);
        wait(NULL);
        printf("Final value of i: %d\n", *i);
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        printf("Fork failed!\n");
        return 1;
    }

    return 0;
}
