// 6-bpf_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int map_created = 0;
atomic_int map_updated = 0;

void *thread_bpf(void *arg) {
    atomic_store_explicit(&map_created, 1, memory_order_relaxed);
    atomic_store_explicit(&map_updated, 1, memory_order_relaxed);
    return NULL;
}

void *thread_sock(void *arg) {
    int created = atomic_load_explicit(&map_created, memory_order_relaxed);
    int updated = atomic_load_explicit(&map_updated, memory_order_relaxed);
    if (created == 1 && updated == 0) assert(0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_sock, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}