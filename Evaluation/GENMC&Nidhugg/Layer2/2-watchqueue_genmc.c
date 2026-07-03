// 2-watchqueue_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int size_set = 0;
atomic_int queue_size = 0;

void *thread_ioctl(void *arg) {
    atomic_store_explicit(&queue_size, 16, memory_order_relaxed);
    atomic_store_explicit(&size_set, 1, memory_order_relaxed);
    return NULL;
}

void *thread_read(void *arg) {
    int set = atomic_load_explicit(&size_set, memory_order_relaxed);
    if (set) {
        int sz = atomic_load_explicit(&queue_size, memory_order_relaxed);
        if (sz == 0) assert(0);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ioctl, NULL);
    pthread_create(&t2, NULL, thread_read, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}