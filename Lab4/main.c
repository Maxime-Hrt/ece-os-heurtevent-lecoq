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
    float results[4];
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

void task1(struct shared_data *s, float a, float b);
void task2(struct shared_data *s, float c, float d);
void task3(struct shared_data *s, float e, float f);
void task4(struct shared_data *s);

int main() {
    int status;

    struct shared_data *shared_data = mmap(NULL, sizeof(struct shared_data), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }
    shared_data_init(shared_data, 1);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        task1(shared_data, 1.0, 2.0);  // Replace with your values for a and b
        exit(0);
    }
    pid_t pid2 = fork();
    if (pid2 == 0) {
        task2(shared_data, 3.0, 4.0);  // Replace with your values for c and d
        exit(0);
    }
    pid_t pid3 = fork();
    if (pid3 == 0) {
        task3(shared_data, 5.0, 6.0);  // Replace with your values for e and f
        exit(0);
    }
    pid_t pid4 = fork();
    if (pid4 == 0) {
        task4(shared_data);
        exit(0);
    }

    waitpid(pid1, &status, 0);
    waitpid(pid2, &status, 0);
    waitpid(pid3, &status, 0);
    waitpid(pid4, &status, 0);

    shared_data_end(shared_data);
    munmap(shared_data, sizeof(struct shared_data));
    return 0;
}

void task1(struct shared_data *s, float a, float b) {
    float result = a + b;
    s->results[0] = result;
    shared_data_post(s, s->sem2);  // Signal next task
}

void task2(struct shared_data *s, float c, float d) {
    shared_data_wait(s, s->sem2);
    float result = c - d;
    s->results[1] = result;
    shared_data_post(s, s->sem3);  // Signal next task
}

void task3(struct shared_data *s, float e, float f) {
    shared_data_wait(s, s->sem3);
    float result = e + f;
    s->results[2] = result;
}

void task4(struct shared_data *s) {
    shared_data_wait(s, s->sem2);
    shared_data_wait(s, s->sem3);
    float intermediate = s->results[0] * s->results[1];
    float final_result = intermediate * s->results[2];
    s->results[3] = final_result;
    printf("Final Result: %f\n", final_result);
}
