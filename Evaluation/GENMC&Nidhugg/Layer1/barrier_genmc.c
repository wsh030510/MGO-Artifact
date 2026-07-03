// barrier_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int data = ATOMIC_VAR_INIT(0);
atomic_int flag = ATOMIC_VAR_INIT(0);

void* writer(void* arg) {
    atomic_store_explicit(&data, 42, memory_order_relaxed);
    // Even if original code used Release, for testing whether tool catches missing barrier, we use Relaxed first
    atomic_store_explicit(&flag, 1, memory_order_relaxed);
    return NULL;
}

void* reader(void* arg) {
    if (atomic_load_explicit(&flag, memory_order_relaxed) == 1) {
        assert(atomic_load_explicit(&data, memory_order_relaxed) == 42);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, writer, NULL);
    pthread_create(&t2, NULL, reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}