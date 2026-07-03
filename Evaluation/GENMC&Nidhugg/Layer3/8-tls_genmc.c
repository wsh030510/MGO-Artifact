// 8-tls_genmc.c
#include <stdatomic.h>
#include <assert.h>
#include <pthread.h>
#include <string.h>

// Redefined variable meanings:
// data_ready indicates whether key has been delivered (ready flag)
// data_value indicates the key data itself (0/1 for existence)
atomic_int data_ready = 0;    // original tls_ulp_enabled
atomic_int data_value = 0;    // original tls_key_ready

void setsockopt_stub(int level, int optname, const char* val) {
    if (level == 31) { // SOL_TCP (enable ULP)
        // Here we actually deliver data (key)?
        // To construct message passing, let level 31 write data_value, level 282 write data_ready
        // So here write data_value
        atomic_store_explicit(&data_value, 1, memory_order_relaxed);
    } else if (level == 282) { // SOL_TLS (configure key)
        // Here write data_ready
        atomic_store_explicit(&data_ready, 1, memory_order_relaxed);
    }
}

void epoll_ctl_stub() {
    // Check ready flag
    if (atomic_load_explicit(&data_ready, memory_order_relaxed) == 1) {
        // Data should be ready
        int val = atomic_load_explicit(&data_value, memory_order_relaxed);
        assert(val == 1);
    }
}

void *thread_tls_config(void *arg) {
    setsockopt_stub(31, 0, "tls");   // write data first
    setsockopt_stub(282, 1, "key");  // write flag later
    return NULL;
}

void *thread_io(void *arg) {
    epoll_ctl_stub();
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_tls_config, NULL);
    pthread_create(&t2, NULL, thread_io, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}