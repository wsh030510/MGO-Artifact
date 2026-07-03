// xsk_genmc.c - mmap after close with usage flag
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int resource_valid = 1;
atomic_int mmap_used = 0;
atomic_int close_done = 0;

void *thread_mmap(void *arg) {
    int v = atomic_load_explicit(&resource_valid, memory_order_relaxed);
    if (v) {
        atomic_store_explicit(&mmap_used, 1, memory_order_relaxed);
    } else {
        // mmap attempted while resource already invalid, error
        assert(0);
    }
    return NULL;
}

void *thread_close(void *arg) {
    atomic_store_explicit(&resource_valid, 0, memory_order_relaxed);
    atomic_store_explicit(&close_done, 1, memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_mmap, NULL);
    pthread_create(&t2, NULL, thread_close, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    int close = atomic_load_explicit(&close_done, memory_order_relaxed);
    int mmap = atomic_load_explicit(&mmap_used, memory_order_relaxed);
    // If close completed first but mmap still used, error already triggered in thread_mmap
    return 0;
}