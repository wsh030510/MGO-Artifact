// 8-smc_genmc.c
#include <stdatomic.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

// Simulate internal kernel state
atomic_int smc_mode_enabled = 0;
atomic_int smc_internal_state = 0;

// Stub to simulate kernel setsockopt system call
void setsockopt_stub(const char* protocol) {
    if (strcmp(protocol, "smc") == 0) {
        // Simulate kernel internal logic: first initialize protocol data, then set flag
        atomic_store_explicit(&smc_internal_state, 1, memory_order_relaxed);
        // Vulnerability: Relaxed write may cause flag to be reordered ahead
        atomic_store_explicit(&smc_mode_enabled, 1, memory_order_relaxed);
    }
}

// Stub to simulate kernel sendmmsg system call
void sendmmsg_stub() {
    // Simulate kernel read logic: first check protocol flag
    if (atomic_load_explicit(&smc_mode_enabled, memory_order_relaxed) == 1) {
        // Vulnerability trigger: saw flag but data may not be visible yet due to reordering
        int state = atomic_load_explicit(&smc_internal_state, memory_order_relaxed);
        assert(state == 1);
    }
}

void *thread_ulp(void *arg) {
    setsockopt_stub("smc");
    return NULL;
}

void *thread_send(void *arg) {
    sendmmsg_stub();
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ulp, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}