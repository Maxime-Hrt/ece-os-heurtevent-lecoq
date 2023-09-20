// reader.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <string.h>

#define KEY 4568
#define MAX_SIZE 256

int main() {
    char command[MAX_SIZE];

    printf("Type 'read' to fetch the text from shared memory: ");
    fgets(command, MAX_SIZE, stdin);
    command[strcspn(command, "\n")] = 0;  // Remove the newline character

    if (strcmp(command, "read") == 0) {
        int shmid = shmget(KEY, MAX_SIZE, 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }

        char *shared_mem = (char *) shmat(shmid, NULL, 0);

        printf("Fetched text: %s\n", shared_mem);

        shmdt(shared_mem);
    } else {
        printf("Invalid command. Exiting.\n");
    }

    return 0;
}
