// tso_test_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int x = ATOMIC_VAR_INIT(0);
atomic_int y = ATOMIC_VAR_INIT(0);
atomic_int r1 = ATOMIC_VAR_INIT(0);
atomic_int r2 = ATOMIC_VAR_INIT(0);

void* thread1(void* arg) {
    atomic_store_explicit(&x, 1, memory_order_relaxed);
    atomic_store_explicit(&r1, atomic_load_explicit(&y, memory_order_relaxed), memory_order_relaxed);
    return NULL;
}

void* thread2(void* arg) {
    atomic_store_explicit(&y, 1, memory_order_relaxed);
    atomic_store_explicit(&r2, atomic_load_explicit(&x, memory_order_relaxed), memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1, NULL);
    pthread_create(&t2, NULL, thread2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    // TSO hallmark: both threads may read old value 0
    assert(!(atomic_load_explicit(&r1, memory_order_relaxed) == 0 && 
             atomic_load_explicit(&r2, memory_order_relaxed) == 0));
    return 0;
}