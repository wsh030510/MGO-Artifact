/*
 * MySQL Bug #74833
 * https://bugs.mysql.com/bug.php?id=74833
 */

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// --- Simulate MySQL data structures ---

// Simulate log_state enumeration
#define LOG_CLOSED 0
#define LOG_OPENED 1

// 1. Buggy Definition (Original)
// "volatile" tells the compiler not to cache the value in a register,
// BUT it does NOT tell the CPU hardware to prevent reordering.
volatile int log_state = LOG_CLOSED;

// Simulate protected resource (e.g., IO_CACHE log_file)
int log_file_data = 0;

// Flags controlling test
volatile int start_flag = 0;
volatile int writer_done = 0;

// --- Thread 1: Writer (simulates MYSQL_BIN_LOG::open) ---
void *thread_open_binlog(void *arg) {
    while (1) {
        // Wait for start signal
        while (start_flag == 0);

        // 1. Initialize resource (simulate opening file, initializing cache)
        log_file_data = 999; // Magic number indicating "Initialized"

        // 2. Update state (Publish)
        // --- BUG POINT ---
        // Original code: log_state = LOG_OPENED;
        // On ARM, CPU may reorder this instruction before "log_file_data = 999" execution!
        // Because they are two different memory addresses with no data dependency.
        log_state = LOG_OPENED;

        // (If fixed, an atomic_store_release would be here)

        writer_done = 1;
        while (start_flag == 1); // Wait for reset
    }
    return NULL;
}

// --- Thread 2: Reader (simulates MYSQL_BIN_LOG::close or other user) ---
void *thread_use_binlog(void *arg) {
    long iterations = 0;

    while (1) {
        // Reset environment
        log_state = LOG_CLOSED;
        log_file_data = 0;
        writer_done = 0;

        // Compiler barrier to prevent initialization reordering
        asm volatile("" ::: "memory");

        // Start Writer
        start_flag = 1;

        // 3. Simulate concurrent check
        // We attempt to "peek" before Writer completes
        while (writer_done == 0) {
            
            // --- BUG POINT ---
            // Original code: if (log_state == LOG_OPENED)
            // This is a Volatile Load. It does not guarantee Acquire semantics.
            if (log_state == LOG_OPENED) {
                
                // 4. Access resource
                // If CPU reordered (Writer wrote state first, or Reader read data early),
                // log_file_data may still be 0.
                int data = log_file_data;

                if (data != 999) {
                    printf("[-] OOO Bug Detected at iteration %ld!\n", iterations);
                    printf("    State: LOG_OPENED\n");
                    printf("    Data : %d (Expected 999)\n", data);
                    printf("    Cause: Volatile failed to enforce ordering on ARM.\n");
                    exit(1);
                }
                
                // If correct data read, break current check and proceed to next round
                break;
            }
        }

        start_flag = 0; // Stop Writer
        iterations++;

        if (iterations % 100000 == 0) {
            // printf("[*] Iterations: %ld\n", iterations);
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    printf("[*] Starting MySQL Volatile Bug Reproduction (ID 16)...\n");
    printf("[*] This test simulates the 'volatile log_state' race condition.\n");

    pthread_create(&t1, NULL, thread_open_binlog, NULL);
    pthread_create(&t2, NULL, thread_use_binlog, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}