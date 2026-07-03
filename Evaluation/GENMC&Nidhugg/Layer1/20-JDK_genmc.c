// 20-JDK_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int waiter_state = ATOMIC_VAR_INIT(0); // 0 = ENTER, 1 = RUN
atomic_int semaphore_val = ATOMIC_VAR_INIT(0);

void* thread_exit(void* arg) {
    // Release semaphore
    atomic_store_explicit(&semaphore_val, 1, memory_order_relaxed);
    // Vulnerability: missing loadstore barrier, causing state update to become visible first
    atomic_store_explicit(&waiter_state, 1, memory_order_relaxed);
    return NULL;
}

void* thread_enter(void* arg) {
    if (atomic_load_explicit(&waiter_state, memory_order_relaxed) == 1) {
        // If state is set to RUN, exit must have been called, semaphore must be 1
        assert(atomic_load_explicit(&semaphore_val, memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_exit, NULL);
    pthread_create(&t2, NULL, thread_enter, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}