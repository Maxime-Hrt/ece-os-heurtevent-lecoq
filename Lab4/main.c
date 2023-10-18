#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <semaphore.h>

typedef struct {
    int i;
    sem_t sem;
} shared_data_t;

void increment(int *var) {
    (*var)++;
}

void decrement(int *var) {
    (*var)--;
}

int main() {
    // Create shared memory segment
    shared_data_t *shared_data = mmap(NULL, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_data->i = 65;
    sem_init(&shared_data->sem, 1, 1);  // Initialize semaphore

    pid_t pid = fork();

    if (pid == 0) {  // Child process
        sem_wait(&shared_data->sem);  // Acquire semaphore
        increment(&shared_data->i);
        sem_post(&shared_data->sem);  // Release semaphore
        exit(0);
    } else if (pid > 0) {  // Parent process
        sem_wait(&shared_data->sem);  // Acquire semaphore
        decrement(&shared_data->i);
        sem_post(&shared_data->sem);  // Release semaphore
        wait(NULL);  // Wait for child process to finish
        printf("Final value of i: %d\n", shared_data->i);  // Should print 65
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    sem_destroy(&shared_data->sem);  // Destroy semaphore
    munmap(shared_data, sizeof(shared_data_t));  // Unmap shared memory
    return 0;
}
