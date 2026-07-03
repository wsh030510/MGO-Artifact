// pso_test_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int x = ATOMIC_VAR_INIT(0);
atomic_int y = ATOMIC_VAR_INIT(0);

void* thread_writer(void* arg) {
    atomic_store_explicit(&x, 1, memory_order_relaxed);
    atomic_store_explicit(&y, 1, memory_order_relaxed);
    return NULL;
}

void* thread_reader(void* arg) {
    if (atomic_load_explicit(&y, memory_order_relaxed) == 1) {
        assert(atomic_load_explicit(&x, memory_order_relaxed) == 1);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}