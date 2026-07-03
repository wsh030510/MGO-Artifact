// 3-sel4_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

atomic_int lock_tail = ATOMIC_VAR_INIT(0);
atomic_int nodes_locked[3];
atomic_int in_cs = ATOMIC_VAR_INIT(0);

void* thread_clh(void* arg) {
    int id = (int)(long)arg;
    
    // Set itself as locked (pending)
    atomic_store_explicit(&nodes_locked[id], 1, memory_order_relaxed);
    
    // BUG: only use acquire, causing previous store to be reordered after this
    int pred = atomic_exchange_explicit(&lock_tail, id, memory_order_acquire);
    
    if (pred != 0) {
        // If due to reordering we read old state 0 of predecessor, we incorrectly enter critical section
        if (atomic_load_explicit(&nodes_locked[pred], memory_order_relaxed) == 0) {
            int cs = atomic_fetch_add_explicit(&in_cs, 1, memory_order_relaxed);
            assert(cs == 0); // catch mutex violation
            atomic_fetch_sub_explicit(&in_cs, 1, memory_order_relaxed);
        }
    }
    atomic_store_explicit(&nodes_locked[id], 0, memory_order_release);
    return NULL;
}

int main() {
    atomic_store_explicit(&nodes_locked[1], 0, memory_order_relaxed);
    atomic_store_explicit(&nodes_locked[2], 0, memory_order_relaxed);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_clh, (void*)1);
    pthread_create(&t2, NULL, thread_clh, (void*)2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}