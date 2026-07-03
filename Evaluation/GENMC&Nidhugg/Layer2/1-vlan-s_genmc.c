// 1-vlan-s_genmc.c - genmc version with assertion
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int vlan_configured = 0;
atomic_int success_count = 0;
atomic_int error_count = 0;

void *thread_vlan(void *arg) {
    atomic_store_explicit(&vlan_configured, 1, memory_order_relaxed);
    return NULL;
}

void *thread_hw_write(void *arg) {
    int conf = atomic_load_explicit(&vlan_configured, memory_order_relaxed);
    if (conf) {
        atomic_fetch_add_explicit(&success_count, 1, memory_order_relaxed);
    } else {
        atomic_fetch_add_explicit(&error_count, 1, memory_order_relaxed);
    }
    return NULL;
}

int main() {
    pthread_t t0, t1;
    pthread_create(&t0, NULL, thread_vlan, NULL);
    pthread_create(&t1, NULL, thread_hw_write, NULL);
    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    if (error_count > 0) {
        assert(0);
    }
    return 0;
}