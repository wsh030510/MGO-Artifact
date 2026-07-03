// 1-rds_genmc.c - Address dependency
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int addr_ready = 0;
atomic_int addr_port = 0;

void *thread_bind(void *arg) {
    atomic_store_explicit(&addr_port, 4000, memory_order_relaxed);
    atomic_store_explicit(&addr_ready, 1, memory_order_relaxed);
    return NULL;
}

void *thread_send(void *arg) {
    int ready = atomic_load_explicit(&addr_ready, memory_order_relaxed);
    if (ready) {
        int port = atomic_load_explicit(&addr_port, memory_order_relaxed);
        if (port == 0) assert(0);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bind, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}