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


-----------------

## Solving the Problem : Synchronizing access using semaphores (2):

*Use semaphores to enforce mutual exclusion and solve the race problem in the first exercise
(sem_init (sem_open for macOS users), sem_wait, sem_post)*

1. initialize a semaphore with value 1
2. create two tasks, one should increment the variable and the other one should decrement it
3. run these tasks and display the final value of ‘i’

initialize a semaphore with value 1
```c
typedef struct {
    int i;
    sem_t sem;
} shared_data_t;

sem_init(&shared_data->sem, 1, 1);  // Initialize semaphore
```

create two tasks, one should increment the variable and the other one should decrement it
```c
void increment(shared_data_t *shared_data) {
    sem_wait(&shared_data->sem);  // Wait for semaphore
    shared_data->i++;
    sem_post(&shared_data->sem);  // Signal semaphore
}

void decrement(shared_data_t *shared_data) {
    sem_wait(&shared_data->sem);  // Wait for semaphore
    shared_data->i--;
    sem_post(&shared_data->sem);  // Signal semaphore
}
```

run these tasks and display the final value of ‘i’
```c
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
        increment(&shared_data->i);
        exit(0);
    } else if (pid > 0) {  // Parent process
        decrement(&shared_data->i);
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
```

the result of the execution of the program is:
```
Final value of i: 65
```

The final value of `i` is 65 because the semaphore ensures that the two threads do not interleave. The increment function is executed first and it increases the value of `i` to 66, then the decrement function is executed and it decreases the value of `i` to 65. The semaphore ensures that the two threads do not interleave in a way that leads to the final value of `i` being 66 or 64.


-----------------
*What if we had more than two processes ? Is there something else to do to enforce
mutual exclusion ? Explain and experiment using three processes.*

1. add a third process that increments the variable
2. run these tasks and display the final value of ‘i’

add a third process that increments the variable
```c
pid_t pid2 = fork();
if (pid2 == 0) {  // Child process
    increment(&shared_data->i);
    exit(0);
} else if (pid2 > 0) {  // Parent process
    decrement(&shared_data->i);
    wait(NULL);  // Wait for child process to finish
    wait(NULL);  // Wait for child process to finish
    printf("Final value of i: %d\n", shared_data->i);  // Should print 65
} else {
    perror("fork");
    exit(EXIT_FAILURE);
}
```

run these tasks and display the final value of ‘i’
```
Final value of i: 66
```

**Increased Complexity:**

With more than two processes, the complexity of interactions among processes increases. It becomes more challenging to predict the order in which the processes will access the shared memory.

**Mutual Exclusion:**

Mutual exclusion is crucial to ensure that each process accesses the shared memory atomically. Semaphores are an effective way to guarantee mutual exclusion, allowing only one process at a time to access the shared memory.

**Experimentation:**

In the experimentation with three processes, a semaphore was used to synchronize access to the shared variable `i`. Each process performed an increment or decrement operation on `i`.

The sequence of operations was: increment, decrement, and then increment, leading to a final value of `i` of 66, which is consistent with the sequence of operations performed.

-----------------




