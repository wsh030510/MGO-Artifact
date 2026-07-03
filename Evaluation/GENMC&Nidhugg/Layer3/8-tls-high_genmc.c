// 8-tls-high_genmc.c - genmc version with relaxed memory model
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

// Simulate TLS state: 0 = not enabled, 1 = enabled
atomic_int tls_enabled = 0;

// Counters for operation results
atomic_int success_count = 0;  // write executed after TLS enabled
atomic_int error_count = 0;    // write executed before TLS enabled

// Thread 0: simulate enabling and configuring TLS (corresponds to setsockopt call)
void *thread_tls_config(void *arg) {
    // Two setsockopt calls in original code merged into one state update
    atomic_store_explicit(&tls_enabled, 1, memory_order_relaxed);
    return NULL;
}

// Thread 1: simulate write and epoll operations
void *thread_io(void *arg) {
    // Simulate write(sock_fd, "test_data", 9)
    int enabled = atomic_load_explicit(&tls_enabled, memory_order_relaxed);
    if (enabled) {
        atomic_fetch_add_explicit(&success_count, 1, memory_order_relaxed);
    } else {
        atomic_fetch_add_explicit(&error_count, 1, memory_order_relaxed);
    }

    // Simulate epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, ...)
    // epoll operation itself does not directly depend on TLS state; placeholder for structure

    return NULL;
}

int main() {
    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread_tls_config, NULL);
    pthread_create(&t2, NULL, thread_io, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Detect bug: if error_count > 0, write executed before TLS configuration
    if (error_count > 0) {
        printf("BUG: write executed before TLS configuration\n");
        return 1;   // non-zero indicates bug detected
    }
    return 0;
}