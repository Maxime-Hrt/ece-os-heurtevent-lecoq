# LAB 2: Sharing memory between processes

## Prerequisites:
how to compile a program from the command line:
```shell
$ gcc program.c -o program
```
how to execute a program from the command line:
```shell
$ ./program
```
## Shared memory:
*Each process has its own distinct context and does not share it with other
processes. Memory is where the context is and therefore, if two processes need to
share an information in memory, they need to create this bit of space explicitly. In
this lab, you’ll be expertimenting with the different system functions used to
create such a shared space.*

```c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define KEY 4567
#define PERMS 0660

int main(int argc, char **argv) {
    int id;
    int i;
    int *ptr;
    system("ipcs -m");
    id = shmget(KEY, sizeof(int), IPC_CREAT | PERMS);
    system("ipcs -m");
    ptr = (int *) shmat(id, NULL, 0);
    *ptr = 54;
    i = 54;
    if (fork() == 0) {
        (*ptr)++;
        i++;
        printf("Value of *ptr = %d\nValue of i = %d\n", *ptr, i);
        exit(0);
    } else {
        wait(NULL);
        printf("Value of *ptr = %d\nValue of i = %d\n", *ptr, i);
        shmctl(id, IPC_RMID, NULL);
    }
}
```
*What could you infer from the output regarding the state of i and *ptr?**
```cli
Value of *ptr = 55
Value of i = 55
Value of *ptr = 55
Value of i = 54
```
The value of i is not shared between the two processes, but the value of *ptr is shared between the two processes.

Explain what the functions `shmget()`, `shmat()` do. What ‘s the need for a « KEY
» here ?
* `shmget()` creates a shared memory segment. The key is used to identify the shared memory segment.
* `shmat()` attaches the shared memory segment identified by the key to the address space of the calling process.
* The key is used to identify the shared memory segment.

*Code explanation:*

After variables declaration, we execute the command below with the system function `system()` explained in the previous lab:
```shell
$ ipcs -m
```
The command `ipcs -m` displays information about the shared memory segments. The output is:
```cli
IPC status from <running system> as of Tue Sep 19 22:15:46 CEST 2023
T     ID     KEY        MODE       OWNER    GROUP
Shared Memory:
m 131072 0xca0f6571 --rw------- maxime_hrt    staff
m 131073 0x510fe80b --rw------- maxime_hrt    staff
```
The first column is the type of the IPC object. The second column is the ID of the IPC object. The third column is the key of the IPC object. The fourth column is the mode of the IPC object. The fifth column is the owner of the IPC object. The sixth column is the group of the IPC object.

Then we create a shared memory segment with the function `shmget()`. The first argument is the key of the shared memory segment. The second argument is the size of the shared memory segment. The third argument is the flags. The flags are used to specify the permissions of the shared memory segment. The function returns the ID of the shared memory segment.
```c
id = shmget(KEY, sizeof(int), IPC_CREAT | PERMS);
```
Execute the previous command again as a proof that the shared memory segment has been created:
```cli
IPC status from <running system> as of Tue Sep 19 22:15:46 CEST 2023
T     ID     KEY        MODE       OWNER    GROUP
Shared Memory:
m 131072 0xca0f6571 --rw------- maxime_hrt    staff
m 131073 0x510fe80b --rw------- maxime_hrt    staff
m 131074 0x000011d7 --rw-rw---- maxime_hrt    staff
``` 
The third parameter of the function `shmget()` is the flags. The flags are used to specify the permissions of the shared memory segment. The flags are:
* `IPC_CREAT`: create the shared memory segment if it does not exist.
* `IPC_EXCL`: fail if the shared memory segment already exists.
* `IPC_NOWAIT`: fail if the shared memory segment is in use.
* `IPC_RW`: read and write permission.

Then we attach the shared memory segment to the address space of the calling process with the function `shmat()`. The first argument is the ID of the shared memory segment. The second argument is the address of the shared memory segment. The third argument is the flags. The flags are used to specify the permissions of the shared memory segment. The function returns the address of the shared memory segment.
```c
ptr = (int *) shmat(id, NULL, 0);
```
Then we write the value 54 to the shared memory segment and to the variable i.

Then we create a child process with the function `fork()`. The child process increments the value of the shared memory segment and the value of the variable i. The parent process waits for the child process to terminate. Then the parent process displays the value of the shared memory segment and the value of the variable i. Then the parent process removes the shared memory segment with the function `shmctl()`. The first argument is the ID of the shared memory segment. The second argument is the command. The third argument is the structure `shmid_ds`. The command is `IPC_RMID` to remove the shared memory segment.

---

## Parallel Computing:
*Write a program that computes the folowing expression “(a + b) * (c + d) * (e +
f)” using 3 different processes.*

```c
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
```
**Output:**
```cli
Branch 1: 1 + 2 = 3
Branch 2: 3 + 4 = 7
Branch 3: 5 + 6 = 11
Final result: (1 + 2) * (3 + 4) * (5 + 6) = 231
```

First we create the shared memory segments with the function `shmget()`. The first argument is the key of the shared memory segment. The second argument is the size of the shared memory segment. The third argument is the flags. The flags are used to specify the permissions of the shared memory segment. The function returns the ID of the shared memory segment.
```c
int id1 = shmget(KEY1, sizeof(int), IPC_CREAT | 0666);
int id2 = shmget(KEY2, sizeof(int), IPC_CREAT | 0666);
int id3 = shmget(KEY3, sizeof(int), IPC_CREAT | 0666);
```
Then we create the first child process with the function `fork()`. The first child process creates the second child process with the function `fork()`. The second child process creates the third child process with the function `fork()`. The first child process computes the value of a + b. The second child process computes the value of c + d. The third child process computes the value of e + f. The parent process waits for the first child process to terminate. Then the parent process waits for the second child process to terminate. Then the parent process waits for the third child process to terminate. Then the parent process computes the final result. Then the parent process removes the shared memory segments with the function `shmctl()`. The first argument is the ID of the shared memory segment. The second argument is the command. The third argument is the structure `shmid_ds`. The command is `IPC_RMID` to remove the shared memory segment.

---

## Implementing Copy/Paste between processes: