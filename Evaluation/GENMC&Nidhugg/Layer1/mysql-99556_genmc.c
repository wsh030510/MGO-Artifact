// mysql-99556_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int counter = 0;
atomic_int data = 0;
atomic_int start = 0;

void* writer(void* arg) {
    while (!atomic_load_explicit(&start, memory_order_relaxed));
    atomic_store_explicit(&data, 1, memory_order_relaxed);
    atomic_fetch_add_explicit(&counter, 1, memory_order_relaxed);
    return NULL;
}

void* reader(void* arg) {
    atomic_store_explicit(&start, 1, memory_order_relaxed);
    int c = atomic_load_explicit(&counter, memory_order_relaxed);
    int d = atomic_load_explicit(&data, memory_order_relaxed);
    // If counter increased but data still 0, relaxed reordering occurred
    if (c == 1 && d == 0) assert(0);
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