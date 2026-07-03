// 9-unix_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int bound = 0;
atomic_int listening = 0;

void *thread_setup(void *arg) {
    atomic_store_explicit(&bound, 1, memory_order_relaxed);
    atomic_store_explicit(&listening, 1, memory_order_relaxed);
    return NULL;
}

void *thread_weird_ioctl(void *arg) {
    int b = atomic_load_explicit(&bound, memory_order_relaxed);
    int l = atomic_load_explicit(&listening, memory_order_relaxed);
    if (b == 1 && l == 0) assert(0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup, NULL);
    pthread_create(&t2, NULL, thread_weird_ioctl, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}