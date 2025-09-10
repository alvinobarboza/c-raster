#include <stdio.h>
#include <unistd.h>
#include "pthread.h"

void * test_thread_string(void * arg) {
    for (int i = 0; i < 10; i++) {
        printf("other %d\n", i);
        usleep(700*1000);
    }

    pthread_exit(NULL);
}

void * test_thread(void * arg) {
    for (int i = 0; i < 10; i++) {
        printf("%d\n", i);
        usleep(500*1000);
    }

    pthread_exit(NULL);
}

int main(void) {
    pthread_t thread1;
    pthread_t thread2;
    pthread_create(&thread1, NULL, test_thread, NULL);
    pthread_create(&thread2, NULL, test_thread_string, NULL);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    printf("Hello Rasterizer");

    return 0;
}