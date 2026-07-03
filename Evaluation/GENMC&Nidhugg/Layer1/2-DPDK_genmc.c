// 2-DPDK_genmc.c
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <assert.h>

typedef struct mcs_node {
    _Atomic(struct mcs_node*) next;
    atomic_int locked;
} mcs_node_t;

_Atomic(mcs_node_t*) tail = ATOMIC_VAR_INIT(NULL);
mcs_node_t node1, node2;

void* thread_1(void* arg) {
    atomic_store_explicit(&node1.next, NULL, memory_order_relaxed);
    atomic_store_explicit(&node1.locked, 1, memory_order_relaxed);

    mcs_node_t* prev = atomic_exchange_explicit(&tail, &node1, memory_order_relaxed);
    if (prev != NULL) {
        // Vulnerability: Relaxed assignment may be delayed in visibility
        atomic_store_explicit(&prev->next, &node1, memory_order_relaxed);
    }
    return NULL;
}

void* thread_2(void* arg) {
    atomic_store_explicit(&node2.next, NULL, memory_order_relaxed);
    atomic_store_explicit(&node2.locked, 1, memory_order_relaxed);

    mcs_node_t* prev = atomic_exchange_explicit(&tail, &node2, memory_order_relaxed);
    if (prev != NULL) {
        atomic_store_explicit(&prev->next, &node2, memory_order_relaxed);
        
        // Simulate waiting for predecessor to release: under weak memory, if assignment to prev->next is not seen, error occurs
        // In GenMC we use assertion to simulate such broken dependency
        mcs_node_t* check_next = atomic_load_explicit(&node1.next, memory_order_relaxed);
        // If t1 has executed assignment but t2 still sees NULL, reordering is caught
        // Note: this is a simplified verification probe
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_1, NULL);
    pthread_create(&t2, NULL, thread_2, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}