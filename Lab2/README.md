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