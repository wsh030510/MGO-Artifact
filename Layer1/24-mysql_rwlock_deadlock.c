/*
 * Bug ID: MySQL Bug #94699
 *https://bugs.mysql.com/bug.php?id=94699
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdatomic.h>

// --- Simulate MySQL data types ---
typedef long os_thread_id_t;
#define os_thread_eq(a, b) ((a) == (b))

// --- Shared variables (simulate key fields of rw_lock_t) ---
// Use volatile to prevent compiler from optimizing away reads/writes, ensuring test focuses on CPU reordering behavior
volatile int lock_recursive = 0;
volatile os_thread_id_t lock_writer_thread = 0;

// Variables controlling test flow
volatile int start_flag = 0;
volatile int writer_done = 0;

// --- Thread 1: Writer (simulates lock acquisition process) ---
void *thread_writer(void *arg) {
    os_thread_id_t my_tid = (os_thread_id_t)arg;

    while (1) {
        // 1. Wait for start signal
        while (start_flag == 0);

        // 2. Write writer_thread (first write)
        lock_writer_thread = my_tid;

        // 3. Write Barrier
        // Corresponds to Patch description: "Writer memory barrier is inserted correctly"
        // Ensures writer_thread write is visible to other cores before recursive write
        atomic_thread_fence(memory_order_release); 
        // Or on ARM equivalent: asm volatile("dmb ish" ::: "memory");

        // 4. Write recursive (later write)
        lock_recursive = 1;

        // Mark write complete
        writer_done = 1;
        
        // Wait for reset signal
        while (start_flag == 1);
    }
    return NULL;
}

// --- Thread 2: Reader (simulates buggy check logic) ---
void *thread_reader(void *arg) {
    os_thread_id_t writer_tid = (os_thread_id_t)arg; // The Writer ID we expect to see

    long iterations = 0;
    
    while (1) {
        // 1. Start Writer
        lock_recursive = 0;
        lock_writer_thread = 0;
        writer_done = 0;
        
        // Compiler barrier to prevent initialization from being reordered below
        asm volatile("" ::: "memory"); 

        start_flag = 1; // Writer starts running now

        // 2. Simulate Bug code segment (Busy Loop check)
        // We attempt to read while Writer hasn't fully completed, creating a race window
        while (writer_done == 0) {
            
            // --- BUG SCENE STARTS ---
            // Corresponds to: if (... && lock->recursive && os_thread_eq(lock->writer_thread, thread_id))
            
            // A. Read recursive
            int r_recursive = lock_recursive;

            // --- MISSING RMB (Missing read barrier) ---
            // Patch fix added here: os_rmb; 
            // Without barrier, CPU may reorder Load-Load here
            
            // B. Read writer_thread
            os_thread_id_t r_writer = lock_writer_thread;

            // --- Consistency check ---
            // If we see recursive=1 (later write), causality dictates we must also see writer_thread=CorrectID (earlier write).
            // If r_recursive is 1 but r_writer is still 0, reordering occurred.
            if (r_recursive == 1 && r_writer != writer_tid) {
                printf("[-] OOO Bug Detected at iteration %ld!\n", iterations);
                printf("    State detected: recursive=1, but writer_thread=%ld (Expected %ld)\n", 
                       r_writer, writer_tid);
                printf("    Cause: CPU reordered loads. Saw 'recursive' update but not 'writer_thread'.\n");
                exit(1); // Bug found, exit
            }
            
            // If correct state is read, break inner loop and prepare next round
            if (r_recursive == 1 && r_writer == writer_tid) {
                break;
            }
        }

        // 3. Reset, prepare for next round
        start_flag = 0;
        iterations++;
        
        if (iterations % 100000 == 0) {
            // printf("[*] %ld iterations passed...\n", iterations);
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    os_thread_id_t mock_tid = 1001; // Simulate Writer's thread ID

    printf("[*] Starting MySQL RW-Lock Deadlock Reproduction (Bug #94699)...\n");
    printf("[*] This test requires an ARM/Weak Memory CPU to fail.\n");

    // Create Writer thread
    pthread_create(&t1, NULL, thread_writer, (void*)mock_tid);

    // Create Reader thread (observation occurs in this thread)
    // For clarity, we create a dedicated thread
    pthread_create(&t2, NULL, thread_reader, (void*)mock_tid);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}