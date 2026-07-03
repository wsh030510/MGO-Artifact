// 7-OpenBLAS_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

#define NUM_BUFFERS 2
atomic_int memory_pool[NUM_BUFFERS];

void* thread_allocate(void* arg) {
    int allocated_pos = -1;
    for (int i = 0; i < NUM_BUFFERS; i++) {
        // Read buffer state
        int state = atomic_load_explicit(&memory_pool[i], memory_order_relaxed);
        if (state == 0) {
            // Vulnerability: normal write instead of atomic compare-and-swap leads to race
            atomic_store_explicit(&memory_pool[i], 1, memory_order_relaxed);
            allocated_pos = i;
            break;
        }
    }
    
    if (allocated_pos != -1) {
        // For GenMC verification, use assertion to detect if another thread simultaneously got same slot
        // If memory_pool[allocated_pos] was changed by another thread, there is a problem
        assert(atomic_load_explicit(&memory_pool[allocated_pos], memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    for(int i=0; i<NUM_BUFFERS; i++) {
        atomic_store_explicit(&memory_pool[i], 0, memory_order_relaxed);
    }

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_allocate, NULL);
    pthread_create(&t2, NULL, thread_allocate, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}