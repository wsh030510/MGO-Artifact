// watchqueue_genmc.c - Watch queue size vs keyctl, three-step
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int size_set = 0;
atomic_int queue_initialized = 0;
atomic_int watch_added = 0;

void *thread_setup(void *arg) {
    atomic_store_explicit(&size_set, 1, memory_order_relaxed);
    atomic_store_explicit(&queue_initialized, 1, memory_order_relaxed);
    return NULL;
}

void *thread_watch(void *arg) {
    int sz = atomic_load_explicit(&size_set, memory_order_relaxed);
    int init = atomic_load_explicit(&queue_initialized, memory_order_relaxed);
    if (sz == 1 && init == 0) {
        // Queue size set but not initialized, adding watch may cause error
        atomic_store_explicit(&watch_added, 1, memory_order_relaxed);
        assert(0);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_watch, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}