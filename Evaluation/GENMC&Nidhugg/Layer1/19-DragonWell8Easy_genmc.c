// 19-DragonWell8Easy_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int table_data = ATOMIC_VAR_INIT(0);
atomic_int table_ptr = ATOMIC_VAR_INIT(0);

void* thread_creator(void* arg) {
    // Corresponds to globalPackageTable = new PackageEntryTable();
    atomic_store_explicit(&table_data, 42, memory_order_relaxed);
    // BUG: missing release barrier
    atomic_store_explicit(&table_ptr, 1, memory_order_relaxed);
    return NULL;
}

void* thread_accessor(void* arg) {
    // Lock-free read check (first step of double-checked locking)
    if (atomic_load_explicit(&table_ptr, memory_order_relaxed) == 1) {
        // If pointer not NULL, internal data must be initialized
        assert(atomic_load_explicit(&table_data, memory_order_relaxed) == 42);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_creator, NULL);
    pthread_create(&t2, NULL, thread_accessor, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}