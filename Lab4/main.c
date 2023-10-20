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
    shared_data_init(shared_data, 1, 1); // 1 = mutual exclusion, 1 = initial value

    pid_t pid = fork();

    if (pid == 0) {
        increment(shared_data); // Why shared_data->i and not shared_data?  => Because we want to increment the value of i and not the address of i
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