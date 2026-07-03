// Original code
//[54]https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=64d816cba06c
// Improved code
//[19]https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=95bcade33a8a
// Linux kernel code download 4.5
//https://blog.csdn.net/wtl1992/article/details/121739072
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct mcs_node {
    atomic_bool locked;
    struct mcs_node* next;
} mcs_node_t;

typedef struct {
    mcs_node_t* tail;
} qspinlock_t;

#define NUM_THREADS 8 // Define the number of threads
//mcs_node_t nodes[NUM_THREADS]; // Global array storing each thread's mcs_node_t instance
qspinlock_t my_lock;

void qspinlock_init(qspinlock_t* lock) {
    lock->tail = NULL;
}

//void qspin_lock(qspinlock_t* lock, mcs_node_t* mynode) {
//    mynode->next = NULL;
//    atomic_store_explicit(&mynode->locked, true, memory_order_relaxed);
//
//    mcs_node_t* prev = atomic_exchange_explicit(&lock->tail, mynode, memory_order_acq_rel);
//    if (prev != NULL) {
//        prev->next = mynode;
//        _Atomic(bool) flag =  atomic_load_explicit(&mynode->locked, memory_order_acquire);
//        while (flag) {
//            // Busy-wait
//            flag = atomic_load_explicit(&mynode->locked, memory_order_acquire);
//        }
//    }
//}

// Buggy version
void qspin_lock(qspinlock_t* lock, mcs_node_t* mynode, int i) {
    mynode->next = NULL;
    mynode->locked = true;
    printf("1W locked  %d\n", i);
    mcs_node_t* prev  = __sync_lock_test_and_set(&lock->tail, mynode);

    if (prev != NULL) {
        prev->next = mynode;
        // 1,2 may order
        printf("2W next  %d\n", i);
        while (mynode->locked) {
            // Busy-wait
        }
    }
}

void qspin_unlock(qspinlock_t* lock, mcs_node_t* mynode, int i) {
    printf("3R next   %d\n", i);
    if (!mynode->next) {
        if (__sync_bool_compare_and_swap(&lock->tail, mynode, NULL)) {
            return;
        }
        while (!mynode->next) {
            // Wait for the successor to appear
        }
    }
    printf("4W locked   %d\n", i);
    __atomic_thread_fence(__ATOMIC_RELEASE);
    mynode->next->locked = false;
}

// static driver
//void* enter_critical_section(void* arg) {
//    int thread_id = *((int*)arg);
//
//    printf("Thread started %d\n", thread_id);
//    qspin_lock(&my_lock, &global_node, thread_id);
//    // Critical section start
//    return NULL;
//}
//
//void* exit_critical_section(void* arg) {
//    int thread_id = *((int*)arg);
//
//    // Critical section end
//    int sleep_time = rand() % 5 + 1; // Generate random sleep time between 1 and 5 seconds
//    sleep(sleep_time); // Sleep for random time
//
//    qspin_unlock(&my_lock, &global_node, thread_id);
//    printf("Thread finished %d\n", thread_id);
//    return NULL;
//}

//int main() {
//    // Initialize the lock
//    qspinlock_init(&my_lock);
//    pthread_t thread1, thread2;
//    int thread_id = 1;
//    // Create threads
//    pthread_create(&thread1, NULL, enter_critical_section, &thread_id);
//    pthread_create(&thread2, NULL, exit_critical_section, &thread_id);
//
//
//    pthread_join(thread1, NULL);
//    pthread_join(thread2, NULL);
//    return 0;
//}

//=========dynamic

void* thread_func(void *arg) {
    int i = *(int*)arg;
    mcs_node_t mynode;
    qspin_lock(&my_lock, &mynode, i);
    if (i == 0){
        usleep(2300); // usleep takes microseconds, so multiply by 1000
    }
    printf("Thread %d in critical section for ms\n", i);

    qspin_unlock(&my_lock, &mynode, i);
    return NULL;
}

int main() {
    qspinlock_init(&my_lock);
    pthread_t threads[10];
    int thread_ids[10];
    for (int i = 0; i < 2; i++) {
        thread_ids[i] = i;
        if (i == 1){
            usleep(100); // usleep takes microseconds, so multiply by 1000
        }
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}