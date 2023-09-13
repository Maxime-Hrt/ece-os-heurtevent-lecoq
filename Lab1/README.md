# LAB 1: Programmer's view - Creating Processes

## Prerequisites:
how to compile a program from the command line:
```shell
$ gcc program.c -o program
```

how to execute a program from the command line:
```shell
$ ./program
```
## Creating and Running a Process (1):
*What happens after work a fork call? How are parent and child differentiated?
After a fork system call is made, a new child process is created as a duplicate of the parent process. Here's what happens and how parent and child processes are differentiated*

1. **Creation of Child Process:** When the fork system call is executed, the operating system creates a new process that is an exact copy of the parent process. This includes duplicating the code, data, file descriptors, and other attributes of the parent process.

2. **Return Value:** In the parent process, the fork system call returns the child's process ID (PID), which is a positive integer greater than zero. In the child process, it returns 0.

3. **Differentiation:**
   - In the parent process, you can differentiate it from the child process by checking if the return value of fork is greater than 0. This indicates that it's the parent process.
   - In the child process, you can differentiate it from the parent process by checking if the return value of fork is 0. This indicates that it's the child process.

-----------------

*Write a small C program in which the parent process creates a
child processand each displays a different message : I’m the
parent vs I’m the child. Display the process id and the parent
process id for every running process.*

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork(); // Create a child process

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // This code is executed by the child process
        printf("PID value: %d\n", pid);
        printf("I'm the child (PID: %d, Parent PID: %d)\n", getpid(), getppid());
    } else {
        // This code is executed by the parent process
        printf("PID value: %d\n", pid);
        printf("I'm the parent (PID: %d, Child PID: %d)\n", getpid(), pid);
    }

    return 0;
}
```
**Functions explanation:**
- `fork()` is used to create a new process by duplicating the calling process. The new process is referred to as the child process. The calling process is referred to as the parent process.
- `getpid()` returns the process ID (PID) of the calling process.
- `getppid()` returns the process ID of the parent of the calling process.

**Output:**
```cli
PID value: 36556
I'm the parent (PID: 36552, Child PID: 36556)
PID value: 0
I'm the child (PID: 36556, Parent PID: 36552)
```
As a proof of order execution, we can modify the parent process execution with adding a `sleep(3)`. 
```c
printf("PID value: %d\n", pid);
sleep(3);
printf("I'm the parent (PID: %d, Child PID: %d)\n", getpid(), pid);
```
**Output:**
```cli
PID value: 36937
PID value: 0
I'm the child (PID: 36937, Parent PID: 36935)
I'm the parent (PID: 36935, Child PID: 36937)
```
First the parent process is executed, when it reaches the `sleep(3)` function, the child process executed in parallel will be executed before the second instruction in parent process. After 3 seconds, the parent process will be executed again.

-----------------

*Is data shared between parent and child*

No, data is not shared between parent and child. Each process has its own address space. The parent process and child process have separate copies of the data. The child process gets a copy of all the memory segments of the parent process. The child process does not share the stack with the parent process. The child process gets a copy of the stack of the parent process. The child process does not share the heap with the parent process. The child process gets a copy of the heap of the parent process.

```c
int main() {
    int i = 5;
    if (fork() == 0) {
        i++;
        printf("Child value: %d\n", i);
    } else {
        sleep(3);
        printf("Parent value: %d\n", i);
    }
    return 0;
}
```
**Output:**
```cli
Child value: 6
Parent value: 5
```
Child is executed before for the same reason as question above.

-----------------
*Is it possible to create more than one child process ? Show how
using a simple program that creates 2 children for the 1st-level
process (main parent) and a child for one of the 2nd-level
processes (children).*
```c
int main() {
    printf("1st-level parent process. PID: %d\n", getpid());

    pid_t child1_pid = fork(); // Create the first child

    if (child1_pid == 0) {
        printf("1st-level child process 1. PID: %d, Parent PID: %d\n", getpid(), getppid());

        pid_t grandchild_pid = fork(); // Create a child for the first child

        if (grandchild_pid == 0) {
            printf("2nd-level grandchild process. PID: %d, Parent PID: %d\n", getpid(), getppid());
        }
        return 0; // Exit the first child
    }

    pid_t child2_pid = fork(); // Create the second child

    if (child2_pid == 0) {
        printf("1st-level child process 2. PID: %d, Parent PID: %d\n", getpid(), getppid());
    }

    // Wait for all child processes to finish
    for (int i = 0; i < 2; i++) {
        wait(NULL);
    }

    return 0; // The main parent process exits
}
```
`wait` is used to wait for state changes in a child of the calling process, 
and obtain information about the child whose state has changed. 
A state change is considered to be: the child terminated; the child was stopped 
by a signal; or the child was resumed by a signal. In our case, we wait for a child to terminate.

**Output:**
```cli
1st-level parent process. PID: 50154
1st-level child process 1. PID: 50157, Parent PID: 50154
1st-level child process 2. PID: 50158, Parent PID: 50154
2nd-level grandchild process. PID: 50159, Parent PID: 50157
```
-----------------

## Creating and Running a Process (2):
