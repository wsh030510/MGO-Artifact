// 26-ompi_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int hdr_size = ATOMIC_VAR_INIT(0);
atomic_int global_hdr_ptr = ATOMIC_VAR_INIT(0);

void* thread_writer(void* arg) {
    // Simulate C++ constructor assignment
    atomic_store_explicit(&hdr_size, 1024, memory_order_relaxed);
    // Missing wmb(), directly publish pointer
    atomic_store_explicit(&global_hdr_ptr, 1, memory_order_relaxed);
    return NULL;
}

void* thread_reader(void* arg) {
    if (atomic_load_explicit(&global_hdr_ptr, memory_order_relaxed) == 1) {
        // If pointer is published, object internal data must be visible
        assert(atomic_load_explicit(&hdr_size, memory_order_relaxed) == 1024);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}