/*
 * Bug: crossbeam's SeqLock may fail to detect concurrent writer on weak memory models
 * https://github.com/crossbeam-rs/crossbeam/issues/859
 */

#include <pthread.h>
#include <stdatomic.h>
#include <unistd.h>

// SeqLock state (timestamp)
atomic_int state = 0;

// Protected data (non-atomic)
int shared = 0;

// Reader thread local cache
int cached_value = 0;
int last_stamp = 0;

void *reader(void *arg) {
    // optimistic_read() phase
    int stamp = atomic_load_explicit(&state, memory_order_acquire);
    if (stamp != 0) {
        // writer has already run before reader entered; this branch is not relevant for this test
        return NULL;
    }

    // Simulate entering critical section, giving writer some time to execute concurrently
    usleep(1000);   // Allow writer a chance to modify shared and state

    // Unprotected read of shared (may read writer's written value 1)
    int val = shared;   // Note: This is a data race, technically UB, but kept for demonstration

    // validate_read(): Read state with Relaxed, may miss writer's update
    int new_stamp = atomic_load_explicit(&state, memory_order_relaxed);

    // If new_stamp == 0, it means validate_read believes no writer has run
    // But val may already be 1 (from writer); bug occurs
    if (new_stamp == stamp) {
        // Mistakenly thinks no concurrent writer, uses val
        // Should actually panic because val may have been corrupted by writer
        if (val == 1) {
            // Bug triggered: read writer's value, but validate_read did not detect it
            // Here we could set a flag, but only logic is demonstrated
        }
    }
    return NULL;
}

void *writer(void *arg) {
    // Modify state to indicate write start
    atomic_exchange_explicit(&state, 1, memory_order_acquire);
    // Write shared data
    shared = 1;
    // Ideally after writing, state should be incremented (e.g., with Release write 2), but simplified: writer only changes state from 0 to 1
    // True SeqLock would increment state after write; this does not affect the essence of the bug
    return NULL;
}

int main() {
    pthread_t r, w;
    pthread_create(&r, NULL, reader, NULL);
    pthread_create(&w, NULL, writer, NULL);
    pthread_join(r, NULL);
    pthread_join(w, NULL);
    return 0;
}