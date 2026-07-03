// https://bugs.mysql.com/bug.php?id=87293

#include <stdatomic.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

// Define mutex states
#define MUTEX_STATE_UNLOCKED 0
#define MUTEX_STATE_LOCKED 1
#define MUTEX_STATE_WAITERS 2

// Declare an atomic lock variable
_Atomic int m_lock_word = MUTEX_STATE_UNLOCKED;

// Attempt to acquire lock
bool try_lock() {
    int expected = MUTEX_STATE_UNLOCKED;
    return atomic_compare_exchange_strong(&m_lock_word, &expected, MUTEX_STATE_LOCKED);
}

int TAS(_Atomic int *lock_word, int new_val) {
    // Directly return the value before the operation
    return atomic_exchange(lock_word, new_val);
}

int unlock() {
    return TAS(&m_lock_word, MUTEX_STATE_UNLOCKED);
}

// Acquire lock
void enter() {
    if (!try_lock()) {
        // In real situations, some delay or other logic might be needed here to prevent busy-waiting
    }
}

// Simulated os_rmb function
void os_rmb() {
    // Implementation details omitted, typically a memory barrier operation
}

int state() {
    return atomic_load(&m_lock_word);
}

// Simulate waking waiting threads
void signal() {
    // Implementation details omitted, can be implemented based on specific requirements
    // For example, use condition variables to notify waiting threads
}

void myExit() {
    // Read memory barrier to ensure operation order
    os_rmb();
    // wmb

    if (state() == MUTEX_STATE_WAITERS) {
        m_lock_word = MUTEX_STATE_UNLOCKED;
    } else if (unlock() == MUTEX_STATE_LOCKED) {
        // If no threads are waiting, there's no need to wake
        return;
    }

    // Wake waiting threads
    signal();
}

// Worker function executed by each thread
void* worker(void* arg) {
    enter();
    printf("Thread %ld: Mutex acquired.\n", (long)arg);
    myExit();
    // Simulate some work
    printf("Thread %ld: Mutex released.\n", (long)arg);
    return NULL;
}

int main() {
    const int NUM_THREADS = 2;
    pthread_t threads[NUM_THREADS];

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}