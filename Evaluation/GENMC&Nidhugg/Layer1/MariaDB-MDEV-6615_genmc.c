// MariaDB-MDEV-6615_genmc.c
/*
 * MariaDB MDEV-6615 GenMC test version (simplified lock-free version)
 * All atomic operations use memory_order_relaxed
 * Expected to trigger assert(0) under ARM/POWER model
 */
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int data = 0;
atomic_int flag = 0;
atomic_int start = 0;

void *writer(void *arg) {
    // Wait for start signal
    while (!atomic_load_explicit(&start, memory_order_relaxed));
    // Write data then flag (may be reordered)
    atomic_store_explicit(&data, 42, memory_order_relaxed);
    atomic_store_explicit(&flag, 1, memory_order_relaxed);
    return NULL;
}

void *reader(void *arg) {
    while (!atomic_load_explicit(&start, memory_order_relaxed));
    // Read flag then data (may be reordered)
    int f = atomic_load_explicit(&flag, memory_order_relaxed);
    int d = atomic_load_explicit(&data, memory_order_relaxed);
    // If flag=1 but data != 42, reordering occurred
    if (f == 1 && d != 42) {
        assert(0);   // weak memory vulnerability triggered
    }
    return NULL;
}

int main() {
    pthread_t w, r;
    atomic_store_explicit(&start, 0, memory_order_relaxed);
    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r, NULL, reader, NULL);
    atomic_store_explicit(&start, 1, memory_order_relaxed);
    pthread_join(w, NULL);
    pthread_join(r, NULL);
    return 0;
}