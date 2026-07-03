// 7-xdp2_genmc.c - genmc version with assertion
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int bpf_loaded = 0;
atomic_int success_count = 0;
atomic_int error_count = 0;

void *thread_bpf(void *arg) {
    atomic_store_explicit(&bpf_loaded, 1, memory_order_relaxed);
    return NULL;
}

void *thread_tx_ring(void *arg) {
    int loaded = atomic_load_explicit(&bpf_loaded, memory_order_relaxed);
    if (loaded) {
        atomic_fetch_add_explicit(&success_count, 1, memory_order_relaxed);
    } else {
        atomic_fetch_add_explicit(&error_count, 1, memory_order_relaxed);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_tx_ring, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    if (error_count > 0) {
        assert(0);
    }
    return 0;
}