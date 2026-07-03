// mysql-ut-realx_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int ready = 0;
atomic_int value = 0;
atomic_int start = 0;

void* writer(void* arg) {
    while (!atomic_load_explicit(&start, memory_order_relaxed));
    atomic_store_explicit(&value, 42, memory_order_relaxed);
    atomic_store_explicit(&ready, 1, memory_order_relaxed);
    return NULL;
}

void* reader(void* arg) {
    atomic_store_explicit(&start, 1, memory_order_relaxed);
    int r = atomic_load_explicit(&ready, memory_order_relaxed);
    int v = atomic_load_explicit(&value, memory_order_relaxed);
    // If ready is 1 but value is not 42, the two stores were reordered
    if (r == 1 && v != 42) assert(0);
    return NULL;
}

int main() {
    pthread_t w, r;
    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r, NULL, reader, NULL);
    pthread_join(w, NULL);
    pthread_join(r, NULL);
    return 0;
}