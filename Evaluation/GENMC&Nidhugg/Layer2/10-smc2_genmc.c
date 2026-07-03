// 10-smc2_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int opt_value = 0;
atomic_int opt_ready = 0;

void *thread_opt(void *arg) {
    atomic_store_explicit(&opt_value, 1, memory_order_relaxed);
    atomic_store_explicit(&opt_ready, 1, memory_order_relaxed);
    return NULL;
}

void *thread_send(void *arg) {
    int ready = atomic_load_explicit(&opt_ready, memory_order_relaxed);
    if (ready) {
        int val = atomic_load_explicit(&opt_value, memory_order_relaxed);
        if (val == 0) assert(0);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_opt, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}