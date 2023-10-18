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
void increment(void *arg) {
    int *i = (int *) arg;
    *i = *i + 1;
}

void decrement(void *arg) {
    int *i = (int *) arg;
    *i = *i - 1;
}
```

Run these tasks and display the final value of ‘i’
```c
int main() {
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, increment, &i);
    pthread_create(&tid2, NULL, decrement, &i);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("i = %d\n", i);
    return 0;
}
```

the result of the execution of the program is:
```
i = 65
```

In the Code 1 that I provided you with, the two threads (one for incrementing and the other for decrementing) are supposed to execute almost simultaneously. However, due to the speed of the increment and decrement operations, it's very likely that one of the threads finishes its execution before the other one starts. Thus, it's probable that you get the final value of `i` as 65, as one thread increases the value of `i` to 66 and then the other thread decreases it back to 65.

This does not adequately demonstrate the problem of the race condition that can occur when there's concurrent access to shared memory without appropriate synchronization. The race condition problem is more evident in scenarios where there are additional instructions that can cause delays, thus allowing the threads to interleave in a way that leads to unexpected results.




