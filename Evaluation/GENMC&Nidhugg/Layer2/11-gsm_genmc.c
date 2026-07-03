// 11-gsm_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int ldisc_requested = 0;
atomic_int ldisc_active = 0;

void *thread_ldisc(void *arg) {
    atomic_store_explicit(&ldisc_requested, 1, memory_order_relaxed);
    atomic_store_explicit(&ldisc_active, 1, memory_order_relaxed);
    return NULL;
}

void *thread_term(void *arg) {
    int req = atomic_load_explicit(&ldisc_requested, memory_order_relaxed);
    int act = atomic_load_explicit(&ldisc_active, memory_order_relaxed);
    if (req == 1 && act == 0) assert(0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ldisc, NULL);
    pthread_create(&t2, NULL, thread_term, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}