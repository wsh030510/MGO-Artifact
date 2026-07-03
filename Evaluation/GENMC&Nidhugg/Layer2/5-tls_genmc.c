// 5-tls_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int tls_active = 0;
atomic_int io_setup_done = 0;

void *thread_io(void *arg) {
    atomic_store_explicit(&io_setup_done, 1, memory_order_relaxed);
    int tls = atomic_load_explicit(&tls_active, memory_order_relaxed);
    if (tls == 0 && io_setup_done) assert(0);  // I/O executed before TLS
    return NULL;
}

void *thread_tls(void *arg) {
    atomic_store_explicit(&tls_active, 1, memory_order_relaxed);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_io, NULL);
    pthread_create(&t2, NULL, thread_tls, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}