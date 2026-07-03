/*
 *DPDK MCS Lock Hang
 * http://patches.dpdk.org/patch/75983/
 */

#include <pthread.h>
#include <stdio.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdlib.h>

// Simulate MCS lock node structure
typedef struct mcs_node {
    struct mcs_node *next;
    int locked;
} mcs_node_t;

// Define two nodes: predecessor (prev) and current (me)
mcs_node_t node_prev;
mcs_node_t node_me;

// Thread 1: Simulates Successor (thread attempting to acquire lock)
// Corresponds to rte_mcslock_lock function logic in the screenshot
void *thread_successor_lock(void *arg) {
    // 1. Initialize own node
    node_me.next = NULL;
    node_me.locked = 1; // Prepare to spin waiting

    __atomic_store_n(&node_prev.next, &node_me, __ATOMIC_RELAXED);

    return NULL;
}

// Thread 2: Simulates Predecessor (thread currently holding lock and preparing to release)
// Corresponds to logic in rte_mcslock_unlock handling "next == NULL"
void *thread_predecessor_unlock(void *arg) {
    // Simulate predecessor node holding lock
    node_prev.next = NULL;
    
    // Launch successor thread to link to us
    pthread_t t;
    pthread_create(&t, NULL, thread_successor_lock, NULL);

    // --- Simulate race condition during unlock ---
    // Predecessor node attempts to release lock. It checks next pointer.
    // If next is NULL (but there is already a new node on the global tail), it must wait for next to be updated.
    
    long cycles = 0;
    // This is where the "Hang" occurs
    while (__atomic_load_n(&node_prev.next, __ATOMIC_ACQUIRE) == NULL) {
        cycles++;
        // For demonstration, if loop count exceeds threshold, we consider visibility delay/deadlock triggered
        if (cycles > 10000000) { 
            printf("[-] Bug Reproduced: Predecessor HANGED waiting for next pointer!\n");
            printf("    (Due to __ATOMIC_RELAXED visibility latency on Weak Memory)\n");
            exit(1); // Force exit to demonstrate failure
        }
    }
    
    printf("[+] Success: Predecessor saw the link. No Hang.\n");
    pthread_join(t, NULL);
    return NULL;
}

int main() {
    printf("[*] Starting DPDK MCS Lock Reproduction...\n");
    printf("[*] Target: 'fix mcslock hang on weak memory'\n");
    
    thread_predecessor_unlock(NULL);
    
    return 0;
}