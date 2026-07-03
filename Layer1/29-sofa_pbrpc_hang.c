/*
 * Bug23 (sofa-pbrpc)
 *https://github.com/baidu/sofa-pbrpc/issues/240
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <unistd.h>

// --- Simulate core variables of sofa-pbrpc ---

// _pending_message_count: 0 = no message, 1 = message pending
// Corresponds to Bug description: "relaxed load of _pending_message_count"
atomic_int pending_message_count = 0;

// _send_token: 0 = Free, 1 = Busy
// Corresponds to Bug description: "relaxed load of _send_token"
atomic_int send_token = 1; // Initialize as busy, simulating previous message being sent

// Used to detect if anyone processed the task
atomic_int work_done = 0;

// Synchronization control
volatile int start_barrier = 0;

// --- Thread 1: Producer (simulates try_start_send) ---
void *thread_try_start_send(void *arg) {
    // Wait for start
    while (start_barrier == 0);

    // 1. Simulate putting message into queue and updating count
    // Original: "store/store_release of _pending_message_count"
    atomic_store_explicit(&pending_message_count, 1, memory_order_release);

    // --- BUG: Missing full barrier (Full Fence / DMB ISH) ---
    // On ARM, Store-Release cannot prevent subsequent Load from being reordered before the Store
    
    // 2. Check send token
    // Original: "first performs a relaxed load of _send_token"
    int token = atomic_load_explicit(&send_token, memory_order_relaxed);

    if (token == 0) {
        // Successfully sees channel as free, process task
        atomic_store(&work_done, 1);
    } else {
        // Sees token=1 (Busy), thinks Thread 2 hasn't finished processing, thus gives up
        // "miss the chance to send the current pending message"
    }
    return NULL;
}

// --- Thread 2: Consumer (simulates get_from_pending_queue) ---
void *thread_get_from_queue(void *arg) {
    // Wait for start
    while (start_barrier == 0);

    // 1. Simulate previous message sent completion, release token
    // Original: "store/store_release of _send_token"
    atomic_store_explicit(&send_token, 0, memory_order_release);

    // --- BUG: Missing full barrier (Full Fence) ---
    
    // 2. Check if there are pending messages
    // Original: "performs a relaxed load of _pending_message_count"
    int count = atomic_load_explicit(&pending_message_count, memory_order_relaxed);

    if (count > 0) {
        // Successfully sees new message, process task
        atomic_store(&work_done, 1);
    } else {
        // Sees count=0, thinks Thread 1 hasn't put message, thus gives up
        // "siting idle in _pending_calls until timeout"
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    long iterations = 0;

    printf("[*] Starting sofa-pbrpc Hang Reproduction (SB Pattern)...\n");

    while (1) {
        iterations++;
        
        // Reset state
        atomic_store(&pending_message_count, 0);
        atomic_store(&send_token, 1); // Initial state busy
        atomic_store(&work_done, 0);
        start_barrier = 0;

        // Create threads
        pthread_create(&t1, NULL, thread_try_start_send, NULL);
        pthread_create(&t2, NULL, thread_get_from_queue, NULL);

        // Trigger concurrency
        // Use volatile write as simple compiler barrier
        asm volatile("" ::: "memory");
        start_barrier = 1;

        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        // --- Check result ---
        // If Bug occurs:
        // T1 sees token still 1 (Busy) -> gives up
        // T2 sees count still 0 (Empty) -> gives up
        // work_done remains 0
        if (atomic_load(&work_done) == 0) {
            printf("[-] Bug Detected at iteration %ld!\n", iterations);
            printf("    State: Message Pending (count=1), Channel Free (token=0)\n");
            printf("    But neither thread processed it! (Deadlock/Timeout)\n");
            exit(1);
        }

        if (iterations % 100000 == 0) {
            // printf("Iterations: %ld\n", iterations);
        }
    }
    return 0;
}