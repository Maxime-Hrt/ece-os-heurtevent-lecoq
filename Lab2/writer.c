// writer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define KEY 4568
#define MAX_SIZE 256

int main() {
    char input[MAX_SIZE];

    // Create shared memory segment
    int shmid = shmget(KEY, MAX_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *shared_mem = (char *) shmat(shmid, NULL, 0);

    printf("Enter your text: ");
    fgets(input, MAX_SIZE, stdin);

    strcpy(shared_mem, input);  // Copy the input to shared memory

    printf("Text saved to shared memory. You can now read it from the other process.\n");

    shmdt(shared_mem);

    return 0;
}
