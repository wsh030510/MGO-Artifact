// 0-datarace_genmc.c
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

// Must use C11 atomic_int
atomic_int data = ATOMIC_VAR_INIT(0);
atomic_int flag = ATOMIC_VAR_INIT(0);

void *writer_thread(void *args) {
    // Use memory_order_relaxed to simulate missing release write barrier
    // Under weak memory model, the order of these two stores may be reversed by the CPU
    atomic_store_explicit(&data, 42, memory_order_relaxed);
    atomic_store_explicit(&flag, 1, memory_order_relaxed);
    return NULL;
}

void *reader_thread(void *args) {
    // Use memory_order_relaxed to simulate missing acquire read barrier
    if (atomic_load_explicit(&flag, memory_order_relaxed) == 1) {
        
        // Checkpoint: if flag is already 1, then data must be 42
        int current_data = atomic_load_explicit(&data, memory_order_relaxed);
        
        // If assertion fails, GenMC has caught reordering (flag becomes visible before data)
        assert(current_data == 42);
    }
    return NULL;
}

int main() {
    pthread_t writer, reader;
    
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, NULL);
    
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);
    
    return 0;
}