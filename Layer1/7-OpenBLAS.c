// https://github.com/OpenMathLib/OpenBLAS/issues/2444
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> // For usleep

#define NUM_BUFFERS 192
#define NUM_THREADS 2

int memory[NUM_BUFFERS];

void init_memory() {
    for (int i = 0; i < NUM_BUFFERS; i++) {
        memory[i] = 0; // Initialize all memory buffers as unused
    }
}

int allocate_memory() {
    for (int position = 0; position < NUM_BUFFERS; position++) {
        //__atomic_thread_fence(__ATOMIC_ACQUIRE);
        // Ensures that all read operations (and writes that depend on those reads) after this barrier
        if (memory[position] == 0) {
            memory[position] = 1;
            printf("Allocated memory buffer %d\n", position);
            return position; // Return the allocated memory position
        }
    }
    return -1; // No available memory unit
}

void release_memory(int position) {
    if (position < 0 || position >= NUM_BUFFERS) {
        printf("Invalid memory buffer position %d\n", position);
        return;
    }
    // arm: ensure all writes are finished before other thread takes this memory
    __atomic_thread_fence(__ATOMIC_RELEASE);
    memory[position] = 0;
    printf("Released memory buffer %d\n", position);
}

void* thread_function(void* arg) {
    int allocated_position = allocate_memory();
    if (allocated_position != -1) {
        printf("Do something with memory buffer %d\n", allocated_position);

        // Simulate random delay during operation execution
        unsigned int delay = rand() % 1000000; // Generate a random delay (0 to 999999 microseconds)
        usleep(delay); // Delay in microseconds

        release_memory(allocated_position);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    init_memory();

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}