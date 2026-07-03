// 3-xsk_genmc.c - Use-after-free via refcount
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int refcnt = 1;
atomic_int valid = 1;
atomic_int error = 0;

void *thread_mmap(void *arg) {
    int v = atomic_load_explicit(&valid, memory_order_relaxed);
    if (v) {
        atomic_fetch_add_explicit(&refcnt, 1, memory_order_relaxed);
    } else {
        atomic_store_explicit(&error, 1, memory_order_relaxed);
    }
    return NULL;
}

void *thread_close(void *arg) {
    atomic_store_explicit(&valid, 0, memory_order_relaxed);
    atomic_fetch_sub_explicit(&refcnt, 1, memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_mmap, NULL);
    pthread_create(&t2, NULL, thread_close, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    if (atomic_load_explicit(&error, memory_order_relaxed)) assert(0);
    return 0;
}