// 4-MySQL_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int recursive = ATOMIC_VAR_INIT(0);
atomic_int writer_tid = ATOMIC_VAR_INIT(0);

void* thread_writer(void* arg) {
    // Simulate state update after lock acquisition
    atomic_store_explicit(&writer_tid, 1001, memory_order_relaxed);
    // Vulnerability: no write barrier, recursive=1 may become visible first
    atomic_store_explicit(&recursive, 1, memory_order_relaxed);
    return NULL;
}

void* thread_reader(void* arg) {
    int r = atomic_load_explicit(&recursive, memory_order_relaxed);
    if (r == 1) {
        int tid = atomic_load_explicit(&writer_tid, memory_order_relaxed);
        // Checkpoint: when seeing recursive=1, must also see tid=1001
        assert(tid == 1001);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}