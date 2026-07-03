// 23-includeos_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int desc_address = ATOMIC_VAR_INIT(0);
atomic_int consumed_count = ATOMIC_VAR_INIT(0);

void* thread_refill(void* arg) {
    // Fill packet data
    atomic_store_explicit(&desc_address, 1111, memory_order_relaxed);
    // BUG: missing barrier, directly update cursor
    atomic_store_explicit(&consumed_count, 1, memory_order_relaxed);
    return NULL;
}

void* thread_receive(void* arg) {
    if (atomic_load_explicit(&consumed_count, memory_order_relaxed) == 1) {
        // If cursor shows filled, address data must be visible
        assert(atomic_load_explicit(&desc_address, memory_order_relaxed) == 1111);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_refill, NULL);
    pthread_create(&t2, NULL, thread_receive, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}