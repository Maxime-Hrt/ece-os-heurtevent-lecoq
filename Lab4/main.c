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
    pid_t pid_firefox;
    pid_t pid_emacs;
    pid_t pid_vi;
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

void process1(struct shared_data *s);
void process2(struct shared_data *s);
void process3(struct shared_data *s);

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

    shared_data_end(shared_data);
    munmap(shared_data, sizeof(struct shared_data));
    return 0;
}

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

