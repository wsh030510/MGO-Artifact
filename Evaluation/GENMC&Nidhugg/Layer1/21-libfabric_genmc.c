// 21-libfabric_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int cmd_data = ATOMIC_VAR_INIT(0);
atomic_int resp_status = ATOMIC_VAR_INIT(0);

void* thread_progress(void* arg) {
    // Get pointer and operate on data
    atomic_store_explicit(&cmd_data, 99, memory_order_relaxed);
    
    // BUG: memory_barrier() omitted
    atomic_store_explicit(&resp_status, 1, memory_order_relaxed);
    return NULL;
}

void* thread_peer(void* arg) {
    if (atomic_load_explicit(&resp_status, memory_order_relaxed) == 1) {
        assert(atomic_load_explicit(&cmd_data, memory_order_relaxed) == 99);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_progress, NULL);
    pthread_create(&t2, NULL, thread_peer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}