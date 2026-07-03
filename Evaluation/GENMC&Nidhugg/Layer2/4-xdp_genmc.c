// 4-xdp_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int prog_loaded = 0;
atomic_int tracepoint_attached = 0;

void *thread_bpf(void *arg) {
    atomic_store_explicit(&prog_loaded, 1, memory_order_relaxed);
    atomic_store_explicit(&tracepoint_attached, 1, memory_order_relaxed);
    return NULL;
}

void *thread_poll(void *arg) {
    int prog = atomic_load_explicit(&prog_loaded, memory_order_relaxed);
    int tp = atomic_load_explicit(&tracepoint_attached, memory_order_relaxed);
    if (prog == 1 && tp == 0) assert(0);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_poll, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}