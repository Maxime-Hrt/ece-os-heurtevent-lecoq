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

