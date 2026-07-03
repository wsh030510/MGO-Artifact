// 24-mysql_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int m_lock_word = ATOMIC_VAR_INIT(0);
atomic_int protected_data = ATOMIC_VAR_INIT(0);

void* thread_worker(void* arg) {
    int expected = 0;
    // Simulate try_lock
    while (!atomic_compare_exchange_weak_explicit(&m_lock_word, &expected, 1, memory_order_relaxed, memory_order_relaxed)) {
        expected = 0;
    }

    // Critical section
    int data = atomic_load_explicit(&protected_data, memory_order_relaxed);
    assert(data == 0); // If lock fails, this may read data written by another thread
    atomic_store_explicit(&protected_data, 1, memory_order_relaxed);
    atomic_store_explicit(&protected_data, 0, memory_order_relaxed);

    // Simulate unlock (TAS), missing release barrier
    atomic_exchange_explicit(&m_lock_word, 0, memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_worker, NULL);
    pthread_create(&t2, NULL, thread_worker, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}