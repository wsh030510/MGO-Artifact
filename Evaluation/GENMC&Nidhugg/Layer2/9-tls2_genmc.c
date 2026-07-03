// 9-tls2_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int repair_on = 0;
atomic_int repair_off = 0;

void *thread_repair_connect(void *arg) {
    atomic_store_explicit(&repair_on, 1, memory_order_relaxed);
    // Simulate connect
    atomic_store_explicit(&repair_off, 1, memory_order_relaxed);
    return NULL;
}

void *thread_tls_md5(void *arg) {
    int on = atomic_load_explicit(&repair_on, memory_order_relaxed);
    int off = atomic_load_explicit(&repair_off, memory_order_relaxed);
    if (on == 1 && off == 0) assert(0);  // TLS/MD5 executed before repair finished
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_repair_connect, NULL);
    pthread_create(&t2, NULL, thread_tls_md5, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}