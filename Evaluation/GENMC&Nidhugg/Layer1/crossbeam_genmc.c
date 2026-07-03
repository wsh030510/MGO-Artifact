// crossbeam_genmc.c
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int seq = 0;
atomic_int shared = 0;
atomic_int start = 0;

void* writer(void* arg) {
    while (!atomic_load_explicit(&start, memory_order_relaxed));
    atomic_exchange_explicit(&seq, 1, memory_order_relaxed); // increment before write
    atomic_store_explicit(&shared, 1, memory_order_relaxed);
    // Real SeqLock would increment again after write, but simplified here; vulnerability remains
    return NULL;
}

void* reader(void* arg) {
    atomic_store_explicit(&start, 1, memory_order_relaxed);
    int stamp = atomic_load_explicit(&seq, memory_order_relaxed);
    if (stamp != 0) return NULL; // writer finished first, not relevant for this test
    // Allow writer to run concurrently (fixed loop, Nidhugg can handle)
    for (int i = 0; i < 10; ++i) {
        __asm__ volatile("" ::: "memory"); // compiler barrier only, no scheduling point
    }
    int val = atomic_load_explicit(&shared, memory_order_relaxed);
    int new_stamp = atomic_load_explicit(&seq, memory_order_relaxed);
    // Vulnerability: new_stamp may still be 0 (writer's update not seen), but val may be 1
    if (new_stamp == stamp && val == 1) assert(0);
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