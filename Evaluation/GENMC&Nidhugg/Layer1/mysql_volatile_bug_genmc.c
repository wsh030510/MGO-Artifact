// mysql_volatile_bug_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int log_data = ATOMIC_VAR_INIT(0);
atomic_int log_state = ATOMIC_VAR_INIT(0);

void* thread_logger(void* arg) {
    atomic_store_explicit(&log_data, 999, memory_order_relaxed);
    // Simulate volatile write (underlying is relaxed, no barrier)
    atomic_store_explicit(&log_state, 1, memory_order_relaxed);
    return NULL;
}

void* thread_consumer(void* arg) {
    if (atomic_load_explicit(&log_state, memory_order_relaxed) == 1) {
        // Under weak memory, when state is 1, data may be dirty (0)
        assert(atomic_load_explicit(&log_data, memory_order_relaxed) == 999);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_logger, NULL);
    pthread_create(&t2, NULL, thread_consumer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}