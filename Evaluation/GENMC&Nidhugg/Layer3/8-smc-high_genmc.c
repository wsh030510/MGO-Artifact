// 8-smc-high_genmc.c - genmc version with relaxed memory model
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

// Simulate socket SMC state (0 = not enabled, 1 = enabled)
atomic_int smc_enabled = 0;

// Counters for send results
atomic_int success_count = 0;
atomic_int error_count   = 0;

void *thread_ulp(void *arg) {
    // Simulate setsockopt(sock_fd, SOL_TCP, TCP_ULP, "smc", 4);
    // Enable SMC
    atomic_store_explicit(&smc_enabled, 1, memory_order_relaxed);
    return NULL;
}

void *thread_send(void *arg) {
    // Simulate sendmmsg(sock_fd, ...)
    // Actual behavior depends on whether SMC is enabled
    int enabled = atomic_load_explicit(&smc_enabled, memory_order_relaxed);
    if (enabled) {
        // SMC enabled, send succeeds
        atomic_fetch_add_explicit(&success_count, 1, memory_order_relaxed);
    } else {
        // SMC not enabled, send fails or behaves abnormally (record error)
        atomic_fetch_add_explicit(&error_count, 1, memory_order_relaxed);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_ulp, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Detect bug: if error_count > 0, sendmmsg executed before setsockopt
    if (error_count > 0) {
        printf("BUG: sendmmsg executed before setsockopt\n");
        return 1;   // non-zero indicates bug detected
    }
    return 0;
}