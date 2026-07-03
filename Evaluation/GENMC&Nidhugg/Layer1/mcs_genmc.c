// mcs_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

// Simple atomic variables to simulate lock queuing mechanism
atomic_int tail = ATOMIC_VAR_INIT(0);
atomic_int prev_next = ATOMIC_VAR_INIT(0);

void* thread_successor(void* arg) {
    // Simulate atomic exchange to enqueue
    atomic_store_explicit(&tail, 1, memory_order_relaxed);
    
    // BUG: Relaxed write to next pointer, may not be visible to predecessor
    atomic_store_explicit(&prev_next, 1, memory_order_relaxed);
    return NULL;
}

void* thread_predecessor(void* arg) {
    // If predecessor finds that tail has been registered by another thread (tail == 1)
    if (atomic_load_explicit(&tail, memory_order_relaxed) == 1) {
        // Then it must see the next pointer updated, otherwise will deadlock (hang)
        assert(atomic_load_explicit(&prev_next, memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_successor, NULL);
    pthread_create(&t2, NULL, thread_predecessor, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}