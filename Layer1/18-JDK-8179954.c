// https://bugs.openjdk.org/browse/JDK-8179954

#include <stdio.h>
#include <pthread.h>

// Global variables
volatile int x = 0;
volatile int y = 0;
volatile int r1 = 0;
volatile int r2 = 0;

// Thread 1 function
void* thread1_func(void* arg) {
    x = 1;  // Write x
    // No memory barrier
    r1 = y;  // Read y
    return NULL;
}

// Thread 2 function
void* thread2_func(void* arg) {
    y = 1;  // Write y
    // No memory barrier
    r2 = x;  // Read x
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    // Create thread 1
    pthread_create(&thread1, NULL, thread1_func, NULL);

    // Create thread 2
    pthread_create(&thread2, NULL, thread2_func, NULL);

    // Wait for thread 1 to complete
    pthread_join(thread1, NULL);

    // Wait for thread 2 to complete
    pthread_join(thread2, NULL);

    // Print results
    printf("r1 = %d, r2 = %d\n", r1, r2);

    return 0;
}