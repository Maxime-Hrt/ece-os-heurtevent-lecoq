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
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>

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
needs to be done in order to achieve either error (64 or 66) at will.*


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

1. Set up semaphore for **MacOS** & other **OS**
2. Initialize semaphore with value 1 (**mutual exclusion**)
3. **Create two functions**, one should increment the variable and the other one should decrement it (Synchronize access to shared variable `i`)
4. Run these tasks and display the final value of `i`

Set up semaphore for **MacOS** & other **OS**:
```c
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

/// ========= MACOS SETUP ========= ///
// Struct to hold shared data
struct shared_data {
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif
    int i;
};

// Init function
static inline void
shared_data_init(struct shared_data *s, int mutualExclusion , uint32_t value) {
#ifdef __APPLE__
    dispatch_semaphore_t *sem = &s->sem;
    *sem = dispatch_semaphore_create(value);
#else
    sem_init(&s->sem, mutualExclusion, value);
#endif
}

// Wait function
static inline void
shared_data_wait(struct shared_data *s) {
#ifdef __APPLE__
    dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
    int r;

    do {
        r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

// Post function
static inline void
shared_data_post(struct shared_data *s) {
#ifdef __APPLE__
    dispatch_semaphore_signal(s->sem);
#else
    sem_post(&s->sem);
#endif
}

// Destroy function
static inline void
shared_data_end(struct shared_data *s) {
#ifdef __APPLE__
    dispatch_release(s->sem);
#else
    sem_destroy(&s->sem);
#endif
}
/// ========= END MACOS SETUP ========= ///
```
Because of deprecated functions in MacOS, we need to use `dispatch_semaphore_t` instead of `sem_t` in the `shared_data` struct and replace some semaphore functions with their MacOS equivalents.


Initialize semaphore with value 1 (**mutual exclusion**):
```c
shared_data_init(shared_data, 1, 1);
```
By initializing the semaphore with value 1, we ensure that only one process at a time can access the shared memory.


**Create two functions**, one should increment the variable and the other one should decrement it (Synchronize access to shared variable `i`):
```c
void increment(struct shared_data *s) {
    shared_data_wait(s);
    s->i++;
    shared_data_post(s);
}

void decrement(struct shared_data *s) {
    shared_data_wait(s);
    s->i--;
    shared_data_post(s);
}
```
The `increment` and `decrement` functions use the `shared_data_wait` and `shared_data_post` functions to synchronize access to the shared variable `i`.

Run these tasks and display the final value of `i`:
```c
void increment(struct shared_data *s);
void decrement(struct shared_data *s);

int main() {
    int status;

    struct shared_data *shared_data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    shared_data->i = 65;
    shared_data_init(shared_data, 1, 1);

    pid_t pid = fork();

    if (pid == 0) {
        increment(shared_data);
        exit(0);
    } else if (pid > 0) {
        decrement(shared_data);
        waitpid(pid, &status, 0);
        printf("The value of i is: %d\n", shared_data->i);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    shared_data_end(shared_data);
    munmap(shared_data, sizeof(struct shared_data));
    return 0;
}
```
```cli
The value of i is: 65
```
First, `mmap` is used to create a shared memory region that can be accessed by multiple processes. The `shared_data` struct is used to hold the shared memory region and the semaphore.
Then the `shared_data` struct i variable is initialized to 65 and the semaphore is initialized with value 1.
Next, a child process is created using `fork`. The child process calls the `increment` function and exits.
The parent process calls the `decrement` function and waits for the child process to finish. Then it prints the final value of `i`.
Finally, the semaphore is destroyed and the shared memory region is unmapped.



-----------------
*What if we had more than two processes ? Is there something else to do to enforce
mutual exclusion ? Explain and experiment using three processes.*

1. add a third process that increments the variable
2. run these tasks and display the final value of ‘i’

Add a third process that increments the variable
```c
pid_t pid = fork();

    if (pid == 0) {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            increment(shared_data);
            exit(0);
        } else if (pid2 > 0) {
            decrement(shared_data);
            waitpid(pid2, &status, 0);
        } else {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    } else if (pid > 0) {
        decrement(shared_data);
        waitpid(pid, &status, 0);
        printf("The value of i is: %d\n", shared_data->i);
    } else {
        perror("fork");
        exit(EXIT_FAILURE);
    }
```
```
Final value of i: 64
```

Run these tasks and display the final value of ‘i’

<u>**Increased Complexity:**</u>
With more than two processes, the complexity of interactions among processes increases. It becomes more challenging to predict the order in which the processes will access the shared memory.

<u>**Mutual Exclusion:**</u>
Mutual exclusion is crucial to ensure that each process accesses the shared memory atomically. Semaphores are an effective way to guarantee mutual exclusion, allowing only one process at a time to access the shared memory.

<u>**Experimentation:**</u>
In the experimentation with three processes, a semaphore was used to synchronize access to the shared variable `i`. Each process performed an increment or decrement operation on `i`.
The sequence of operations was: decrement, increment, and then decrement, leading to a final value of `i` of 64, which is consistent with the sequence of operations performed.
-----------------

*A deadlock is a situation in which a process is waiting for some resource held by another
process waiting for it to release another resource, thereby forming a loop of blocked
processes ! Use semaphores to force a deadlock situation using three processes.*

1. Adapt OS setup to **MacOS** & other **OS**
2. Create 3 processes
3. Set up a deadlock situation
4. Run the program

Adapt OS setup to **MacOS** & other **OS**:
```c
#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

/// ========= MACOS SETUP ========= ///
// Struct to hold shared data
struct shared_data {
#ifdef __APPLE__
    dispatch_semaphore_t sem1;
    dispatch_semaphore_t sem2;
    dispatch_semaphore_t sem3;
#else
    sem_t sem1;
    sem_t sem2;
    sem_t sem3;
#endif
};

// Init function
static inline void
shared_data_init(struct shared_data *s, int mutualExclusion) {
#ifdef __APPLE__
    s->sem1 = dispatch_semaphore_create(1);
    s->sem2 = dispatch_semaphore_create(1);
    s->sem3 = dispatch_semaphore_create(1);
#else
    sem_init(&s->sem1, mutualExclusion, value);
    sem_init(&s->sem2, mutualExclusion, value);
    sem_init(&s->sem3, mutualExclusion, value);
#endif
}

// Wait function
static inline void
shared_data_wait(struct shared_data *s, dispatch_semaphore_t sem) {
#ifdef __APPLE__
    dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
    int r;

    do {
        r = sem_wait(&s->sem);
    } while (r == -1 && errno == EINTR);
#endif
}

// Post function
static inline void
shared_data_post(struct shared_data *s, dispatch_semaphore_t sem) {
#ifdef __APPLE__
    dispatch_semaphore_signal(sem);
#else
    sem_post(&s->sem);
#endif
}

// Destroy function
static inline void
shared_data_end(struct shared_data *s) {
#ifdef __APPLE__
    dispatch_release(s->sem1);
    dispatch_release(s->sem2);
    dispatch_release(s->sem3);
#else
    sem_destroy(&s->sem1);
    sem_destroy(&s->sem2);
    sem_destroy(&s->sem3);
#endif
}
/// ========= END MACOS SETUP ========= ///
```
Set up adapted to use 3 semaphores instead of 1.

Create 3 processes:
```c
void process1(struct shared_data *s) {
    shared_data_wait(s, s->sem1);
    printf("Process 1 acquired sem1\n");
    printf("Process 1 waiting for sem2\n");
    shared_data_wait(s, s->sem2);
}

void process2(struct shared_data *s) {
    shared_data_wait(s, s->sem2);
    printf("Process 2 acquired sem2\n");
    printf("Process 2 waiting for sem3\n");
    shared_data_wait(s, s->sem3);
}

void process3(struct shared_data *s) {
    shared_data_wait(s, s->sem3);
    printf("Process 3 acquired sem3\n");
    printf("Process 3 waiting for sem1\n");
    shared_data_wait(s, s->sem1);
}
```
Creation of processes that wait for each other to release a semaphore.

Set up a deadlock situation:
```c
pid_t pid1 = fork();

    if (pid1 == 0) {
        process1(shared_data);
        exit(0);
    } else {
        pid_t pid2 = fork();
        if (pid2 == 0) {
            process2(shared_data);
            exit(0);
        } else {
            pid_t pid3 = fork();
            if (pid3 == 0) {
                process3(shared_data);
                exit(0);
            } else if (pid3 > 0) {
                waitpid(pid3, &status, 0);
            } else {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            waitpid(pid2, &status, 0);
        }
        waitpid(pid1, &status, 0);
    }
```
The deadlock situation is created by having each process wait for a semaphore that is held by another process. 

Run the program:
```cli
Process 1 acquired sem1
Process 1 waiting for sem2
Process 2 acquired sem2
Process 2 waiting for sem3
Process 3 acquired sem3
Process 3 waiting for sem1
```
This is a deadlock situation because none of the processes can proceed without the other processes releasing the semaphores they are waiting for.

---

*Use semaphores to run 3 different applications (firefox, emacs, vi) in a predefined sequence
no matter in which order they are launched.*

1. Add pid in shared data struct
2. Create 3 processes
3. Run the program

Add pid in shared data struct:
```c
// Struct to hold shared data
struct shared_data {
#ifdef __APPLE__
    dispatch_semaphore_t sem1;
    dispatch_semaphore_t sem2;
    dispatch_semaphore_t sem3;
#else
    sem_t sem1;
    sem_t sem2;
    sem_t sem3;
#endif
    pid_t pid_firefox;
    pid_t pid_emacs;
    pid_t pid_vi;
};
```
Add pid in shared data struct to keep track of the processes.

Create 3 processes:
```c
void process1(struct shared_data *s) {
    shared_data_wait(s, s->sem1);
    printf("Launching Figma\n");
    s->pid_firefox = fork();
    if (s->pid_firefox == 0) {
        execlp("/Applications/Figma.app/Contents/MacOS/Figma", "figma", NULL);
        perror("Failed to launch Firefox");
        exit(EXIT_FAILURE);
    }
    shared_data_wait(s, s->sem2);
}

void process2(struct shared_data *s) {
    shared_data_wait(s, s->sem2);
    printf("Launching Calculator\n");
    s->pid_emacs = fork();
    if (s->pid_emacs == 0) {
        execlp("/System/Applications/Calculator.app/Contents/MacOS/Calculator", "calculator", NULL);
        perror("Failed to launch Emacs");
        exit(EXIT_FAILURE);
    }
    shared_data_post(s, s->sem3);
}

void process3(struct shared_data *s) {
    shared_data_wait(s, s->sem3);
    printf("Launching VoiceMemos\n");
    s->pid_vi = fork();
    if (s->pid_vi == 0) {
        // Link might to be changed depending on the OS
        execlp("/System/Applications/VoiceMemos.app/Contents/MacOS/VoiceMemos", "voicememo", NULL);
        perror("Failed to launch Vi");
        exit(EXIT_FAILURE);
    }
}
```
Same processes as before but with the addition of launching applications.

Run the program:
```cli
Launching Figma
Launching Calculator
Launching VoiceMemos
```
The applications are launched in the correct order.

-----------------
