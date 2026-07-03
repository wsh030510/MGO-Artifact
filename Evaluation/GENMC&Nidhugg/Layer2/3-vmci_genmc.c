// 3-vmci_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int version_done = 0;
atomic_int context_done = 0;

void *thread_vmci(void *arg) {
    atomic_store_explicit(&version_done, 1, memory_order_relaxed);
    atomic_store_explicit(&context_done, 1, memory_order_relaxed);
    return NULL;
}

void *thread_epoll(void *arg) {
    int ver = atomic_load_explicit(&version_done, memory_order_relaxed);
    int ctx = atomic_load_explicit(&context_done, memory_order_relaxed);
    if (ver == 1 && ctx == 0) assert(0);  // Saw version done but context not done
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_vmci, NULL);
    pthread_create(&t2, NULL, thread_epoll, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}