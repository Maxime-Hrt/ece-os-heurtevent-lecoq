#include <pthread.h>
#include <stdio.h>

int i = 65;  // Shared variable

void *increment(void *arg) {
    i++;
    return NULL;
}

void *decrement(void *arg) {
    i--;
    return NULL;
}

int main() {
    pthread_t inc_thread, dec_thread;

    pthread_create(&inc_thread, NULL, increment, NULL);
    pthread_create(&dec_thread, NULL, decrement, NULL);

    pthread_join(inc_thread, NULL);
    pthread_join(dec_thread, NULL);

    printf("Final value of i: %d\n", i);  // Expected to print 65
    return 0;
}
