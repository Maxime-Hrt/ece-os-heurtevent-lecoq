# LAB 4: Interprocess Synchronization

## Prerequisites:
how to compile a program from the command line:
```shell
$ gcc program.c -o program
```

how to execute a program from the command line:
```shell
$ ./program
```

-----------------
## Concurrent Access To Shared Memory : Race Problems (1):

*Using two tasks, create a shared variable ‘i’ and initialize it 65; one task
should increment the variable and the other one should decrement it. Run
these tasks and display the final value of ‘i’ ; can you see an error ?*

1. Create a shared variable ‘i’ and initialize it 65
2. Create two tasks, one should increment the variable and the other one should decrement it
3. Run these tasks and display the final value of ‘i’


Create a shared variable ‘i’ and initialize it 65
```c
int i = 65;
```

Create two tasks, one should increment the variable and the other one should decrement it
```c
void increment(int *var) {
        (*var)++;
}

void decrement(int *var) {
    (*var)--;
}
```

Run these tasks and display the final value of ‘i’
```c
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
```

the result of the execution of the program is:
```
i = 65
```

In the Code 1 that I provided you with, the two threads (one for incrementing and the other for decrementing) are supposed to execute almost simultaneously. However, due to the speed of the increment and decrement operations, it's very likely that one of the threads finishes its execution before the other one starts. Thus, it's probable that you get the final value of `i` as 65, as one thread increases the value of `i` to 66 and then the other thread decreases it back to 65.

This does not adequately demonstrate the problem of the race condition that can occur when there's concurrent access to shared memory without appropriate synchronization. The race condition problem is more evident in scenarios where there are additional instructions that can cause delays, thus allowing the threads to interleave in a way that leads to unexpected results.


-----------------
*Replace the previous tasks with the following instructions. Explain what
needs to be done in order to achieve either error (64 or 65) at will.*

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void increment(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg++;
    *var = reg;
}

void decrement(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg--;
    *var = reg;
}

int main() {
    key_t key = ftok("shmfile",65);
    int shmid = shmget(key, sizeof(int), 0666|IPC_CREAT);
    int *i = (int*) shmat(shmid, (void*)0, 0);
    *i = 65;

    pid_t pid = fork();

    if (pid == 0) {  // Child process
        increment(i);
        shmdt(i);
        return 0;
    } else if (pid > 0) {  // Parent process
        decrement(i);
        wait(NULL);  // Wait for child process to finish
        printf("Final value of i: %d\n", *i);  // Could print 64, 65, or 66 depending on the order of operations
        shmctl(shmid, IPC_RMID, NULL);
    } else {
        printf("Fork failed!\n");
        return 1;
    }

    return 0;
}

```


1. Introduce a delay in the increment and decrement functions
2. Run the program and see the final value of `i`
3. Run the program again and see the final value of `i`

```c
void increment(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg++;
    *var = reg;
}
```

```c
void decrement(int *var) {
    int reg = *var;
    sleep(1);  // Introduce delay
    reg--;
    *var = reg;
}
```

We run the program and see the final value of `i`

```
Final value of i: 66
```

the final value of `i` is 66 because the increment function is executed first and it increases the value of `i` to 66, then the decrement function is executed and it decreases the value of `i` to 65. The delay introduced in the increment and decrement functions allows the two threads to interleave in a way that leads to the final value of `i` being 66.

We run the program again and see the final value of `i`

```
Final value of i: 64
```

The final value of `i` is 64 because the decrement function is executed first and it decreases the value of `i` to 64, then the increment function is executed and it increases the value of `i` to 65. The delay introduced in the increment and decrement functions allows the two threads to interleave in a way that leads to the final value of `i` being 64.

