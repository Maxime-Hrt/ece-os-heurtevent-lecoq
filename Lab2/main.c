#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#define KEY_CHILD 8888
#define KEY_GRANDCHILD 9999
#define PERMS 0660

int affectValue(const char *message);
void handleShmatError(int *pointer, const char *message);
void handleShmctlError(int result, const char *message);

int main() {
    int id_child, id_grandChild, status;
    int a, b, c, d, e, f;
    int *p1, *p2;

    system("ipcs -m"); // Display shared memory segments
    id_child = shmget(KEY_CHILD, sizeof(int), IPC_CREAT | PERMS);
    id_grandChild = shmget(KEY_GRANDCHILD, sizeof(int), IPC_CREAT | PERMS);
    system("ipcs -m");

    p1 = (int*) shmat(id_child, NULL, 0);
    handleShmatError(p1, "SHMAT P1 ERROR");
    p2 = (int*) shmat(id_grandChild, NULL, 0);
    handleShmatError(p2, "SHMAT P2 ERROR");

    pid_t child = fork();

    // Error handling
    if (child < 0) {
        perror("child' Fork failed");
        printf("Error code: %d", errno);
    }

    if (child == 0) {
        pid_t grandChild = fork();
        if (grandChild < 0) {
            perror("'grandChild' Fork failed");
            printf("Error code: %d", errno);
        }

        if (grandChild == 0) {
            a = affectValue("Enter the \" A \" value");
            b = affectValue("Enter the \" B \" value");
            *p1 = a + b;
            printf("A + B = %d\n", *p1);
            exit(0);
        } else {
            waitpid(grandChild, &status, 0);
            c = affectValue("Enter the \" C \" value");
            d = affectValue("Enter the \" D \" value");
            *p2 = c + d;
            printf("C + D = %d\n", *p2);

            handleShmctlError(shmctl(id_grandChild, IPC_RMID, NULL), "SHMCTL ERROR GRANDCHILD");
        }
    } else {
        waitpid(child, &status, 0);
        e = affectValue("Enter the \" E \" value");
        f = affectValue("Enter the \" F \" value");
        printf("E + F = %d\n", (e + f));
        printf("======================\n");
        printf("The result is %d\n", ((*p1) * (*p2) * (e + f)));

        handleShmctlError(shmctl(id_child, IPC_RMID, NULL), "SHMCTL ERROR CHILD");
    }
    return 0;
}

int affectValue(const char *message) {
    int var = 0;
    int scanResult;
    printf("%s: ",message);

    scanResult = scanf("%d", &var);
    if (scanResult != 1) {
        printf("ERROR NOT VALID INPUT");
        exit(EXIT_FAILURE);
    }
    return var;
}

void handleShmatError(int *pointer, const char *message) {
    if (pointer == (int*)-1) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}

void handleShmctlError(int result, const char *message) {
    if (result == -1) {
        perror(message);
    }
}
