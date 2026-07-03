// 1-Linuxeasy_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int lock_val = ATOMIC_VAR_INIT(0);
// Use atomic_int to prevent GenMC from reporting trivial data race, focus on logical reordering
atomic_int in_cs = ATOMIC_VAR_INIT(0);

void* thread_func(void* arg) {
    // Simulate lock acquisition missing acquire barrier
    int expected;
    do {
        expected = 0;
    } while (!atomic_compare_exchange_weak_explicit(
                &lock_val, &expected, 1, 
                memory_order_relaxed, memory_order_relaxed));

    // --- Critical section ---
    int current_cs = atomic_fetch_add_explicit(&in_cs, 1, memory_order_relaxed);
    
    // Checkpoint: if mutex fails, current_cs will be > 0 (another thread entered)
    assert(current_cs == 0); 
    
    atomic_fetch_sub_explicit(&in_cs, 1, memory_order_relaxed);
    // --------------

    // Simulate unlock missing release barrier
    atomic_store_explicit(&lock_val, 0, memory_order_relaxed);
    
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}