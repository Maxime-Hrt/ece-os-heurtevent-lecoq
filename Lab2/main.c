#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY1 4568
#define KEY2 4569
#define KEY3 4570

int main() {
    int a,b,c,d,e,f;

    // Input values for a, b, c, d, e, f using scanf
    printf("Enter values for a, b, c, d, e, f: press enter to confirm each value\n");
    scanf("%d %d %d %d %d %d", &a, &b, &c, &d, &e, &f);

    // Create shared memory segments
    int id1 = shmget(KEY1, sizeof(int), IPC_CREAT | 0666);
    int id2 = shmget(KEY2, sizeof(int), IPC_CREAT | 0666);
    int id3 = shmget(KEY3, sizeof(int), IPC_CREAT | 0666);

    pid_t first_fork = fork();
    if (first_fork == 0) {

        pid_t second_fork = fork();
        if (second_fork == 0) {
            // Branch 2: Compute c + d
            int *shared_result2 = (int *)shmat(id2, NULL, 0);
            *shared_result2 = c + d;
            printf("Branch 2: %d + %d = %d\n", c, d, *shared_result2);

            pid_t third_fork = fork();
            if (third_fork == 0) {
                // Branch 3: Compute e + f
                int *shared_result3 = (int *)shmat(id3, NULL, 0);
                *shared_result3 = e + f;
                printf("Branch 3: %d + %d = %d\n", e, f, *shared_result3);
                exit(0);
            }
            wait(NULL); // Wait for branch 3 to finish
            exit(0);
        }
        wait(NULL); // Wait for branch 2 to finish
        exit(0);
    }else{
        // Branch 1: Compute a + b
        int *shared_result1 = (int *)shmat(id1, NULL, 0);
        *shared_result1 = a + b;
        printf("Branch 1: %d + %d = %d\n", a, b, *shared_result1);
    }

    // Wait for branch 1 (and therefore also 2 and 3) to finish
    wait(NULL);

    int *shared_result1 = (int *)shmat(id1, NULL, 0);
    int *shared_result2 = (int *)shmat(id2, NULL, 0);
    int *shared_result3 = (int *)shmat(id3, NULL, 0);

    // Compute the final result
    int final_result = (*shared_result1) * (*shared_result2) * (*shared_result3);
    printf("Final result: (%d + %d) * (%d + %d) * (%d + %d) = %d\n", a, b, c, d, e, f, final_result);

    // Detach from the shared memory segments
    shmdt(shared_result1);
    shmdt(shared_result2);
    shmdt(shared_result3);

    // Remove the shared memory segments
    shmctl(id1, IPC_RMID, NULL);
    shmctl(id2, IPC_RMID, NULL);
    shmctl(id3, IPC_RMID, NULL);

    return 0;
}
