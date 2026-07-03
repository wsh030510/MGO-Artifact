// 4-xsk2_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int rx_ring = 0;
atomic_int fill_ring = 0;
atomic_int bound = 0;

void *thread_setup_ring(void *arg) {
    atomic_store_explicit(&rx_ring, 1, memory_order_relaxed);
    atomic_store_explicit(&fill_ring, 1, memory_order_relaxed);
    atomic_store_explicit(&bound, 1, memory_order_relaxed);
    return NULL;
}

void *thread_recv(void *arg) {
    int rx = atomic_load_explicit(&rx_ring, memory_order_relaxed);
    int fill = atomic_load_explicit(&fill_ring, memory_order_relaxed);
    int b = atomic_load_explicit(&bound, memory_order_relaxed);
    if (rx == 1 && (fill == 0 || b == 0)) assert(0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup_ring, NULL);
    pthread_create(&t2, NULL, thread_recv, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}