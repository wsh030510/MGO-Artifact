/*
 * http://git.dpdk.org/dpdk/tag/?h=v20.11
 */
#include <stdatomic.h>
#include <stddef.h>    // or <stdlib.h> for NULL
#include <sched.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
typedef struct rte_mcslock {
    struct rte_mcslock *next;
    int  locked; /* 1 if the queue locked, 0 otherwise */
} rte_mcslock_t;

/**
 * Take the MCS lock.
 *
 * @param msl
 *   A pointer to the pointer of a MCS lock.
 *   When the lock is initialized or declared, the msl pointer should be
 *   set to NULL.
 * @param me
 *   A pointer to a new node of MCS lock. Each CPU/thread acquiring the
 *   lock should use its 'own node'.
 */
static inline void
rte_mcslock_lock(rte_mcslock_t **msl, rte_mcslock_t *me, int i)
{
    rte_mcslock_t *prev;

    /* Init me node */
    printf("Thread %d: 1: W locked.\n", i);
    __atomic_store_n(&me->locked, 1, __ATOMIC_RELAXED);
    __atomic_store_n(&me->next, NULL, __ATOMIC_RELAXED);

    /* If the queue is empty, the exchange operation is enough to acquire
     * the lock. Hence, the exchange operation requires acquire semantics.
     * The store to me->next above should complete before the node is
     * visible to other CPUs/threads. Hence, the exchange operation requires
     * release semantics as well.
     */
    prev  = __sync_lock_test_and_set(msl, me);

    if (prev == NULL) {
        printf("Thread %d: PRE NULL\n", i);

        /* Queue was empty, no further action required,
         * proceed with lock taken.
         */
        return;
    }
    /* The store to me->next above should also complete before the node is
     * visible to predecessor thread releasing the lock. Hence, the store
     * prev->next also requires release semantics. Note that, for example,
     * on ARM, the release semantics in the exchange operation is not
     * strong as a release fence and is not sufficient to enforce the
     * desired order here.
     */
    printf("Thread %d: 2: W.next \n", i);
    // right
    //atomic_store_explicit(&prev->next, me, memory_order_release);
    // wrong
     prev->next = me;

    /* The while-load of me->locked should not move above the previous
     * store to prev->next. Otherwise it will cause a deadlock. Need a
     * store-load barrier.
     */
    __atomic_thread_fence(__ATOMIC_ACQ_REL);
    /* If the lock has already been acquired, it first atomically
     * places the node at the end of the queue and then proceeds
     * to spin on me->locked until the previous lock holder resets
     * the me->locked using mcslock_unlock().
     */
    while (__atomic_load_n(&me->locked, __ATOMIC_ACQUIRE)) {
        sched_yield();
    }
}

/**
 * Release the MCS lock.
 *
 * @param msl
 *   A pointer to the pointer of a MCS lock.
 * @param me
 *   A pointer to the node of MCS lock passed in rte_mcslock_lock.
 */
static inline void
rte_mcslock_unlock(rte_mcslock_t **msl, rte_mcslock_t *me, int  i)
{
    printf("Thread %d: 3: R.next \n", i);
    /* Check if there are more nodes in the queue. */
    if (me->next  == NULL) {
        printf("Thread %d: NULL \n", i);

        /* No, last member in the queue. */
        rte_mcslock_t *save_me = me;

        /* Release the lock by setting it to NULL */
        if (__atomic_compare_exchange_n(msl, &save_me, NULL, 0, __ATOMIC_RELEASE, __ATOMIC_RELAXED))
            return;

        /* Speculative execution would be allowed to read in the
         * while-loop first. This has the potential to cause a
         * deadlock. Need a load barrier.
         */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        /* More nodes added to the queue by other CPUs.
         * Wait until the next pointer is set.
         */

        while (__atomic_load_n(&me->next, __ATOMIC_RELAXED) == NULL)
            sched_yield();
    }
    printf("Thread %d: 4: W.locked \n", i);
    /* Pass lock to next waiter. */
    __atomic_store_n(&me->next->locked, 0, __ATOMIC_RELEASE);
}

// Assume the definitions of rte_mcslock_t and
// the functions rte_mcslock_lock and rte_mcslock_unlock have been included as above.

// Global MCS lock variable, initialized to NULL
rte_mcslock_t* global_lock = NULL;

// Two global lock node variables
rte_mcslock_t global_node1, global_node2;

//// Lock function
//void* thread_lock(void* arg) {
//    rte_mcslock_t* node = (rte_mcslock_t*)arg;
//
//    // Acquire lock
//    rte_mcslock_lock(&global_lock, node, pthread_self());
//    printf("Thread %lu: Lock acquired.\n", pthread_self());
//    return NULL;
//}
//
//// Unlock function
//void* thread_unlock(void* arg) {
//    rte_mcslock_t* node = (rte_mcslock_t*)arg;
//    // Release lock
//    rte_mcslock_unlock(&global_lock, node, pthread_self());
//    printf("Thread %lu: Lock released.\n", pthread_self());
//    return NULL;
//}
//
//// Main function
//int main() {
//    pthread_t thread1, thread2, thread3, thread4;
//
//    // Initialize nodes
//    global_node1.locked = ATOMIC_VAR_INIT(0); // Indicates this node is initially unlocked
//    global_node1.next = ATOMIC_VAR_INIT(NULL);
//    global_node2.locked = ATOMIC_VAR_INIT(0); // Same as above
//    global_node2.next = ATOMIC_VAR_INIT(NULL);
//
//    // Create threads to perform lock and unlock operations
//    pthread_create(&thread1, NULL, thread_lock, &global_node1);
//    pthread_create(&thread2, NULL, thread_unlock, &global_node1);
//    pthread_create(&thread3, NULL, thread_lock, &global_node2);
//    pthread_create(&thread4, NULL, thread_unlock, &global_node2);
//
//    // Wait for threads to finish
//    pthread_join(thread1, NULL);
//    pthread_join(thread2, NULL);
//    pthread_join(thread3, NULL);
//    pthread_join(thread4, NULL);
//
//    printf("All threads have finished execution.\n");
//
//    return 0;
//}

// ===========dynamic

void* thread_func(void* arg) {
    int thread_id = *(int*)arg;
    rte_mcslock_t* node = (thread_id == 1) ? &global_node1 : &global_node2;

    // Acquire lock
    rte_mcslock_lock(&global_lock, node, thread_id);
    if (thread_id == 0)  usleep(3000); // usleep takes microseconds, so multiply by 1000

    // Critical section code
    printf("Thread %d: start\n", thread_id);
    // Simulate some work
    printf("Thread %d: end\n", thread_id);

    // Release lock
    rte_mcslock_unlock(&global_lock, node, thread_id);

    return NULL;
}

int main() {
    pthread_t threads[2];

    // Initialize global nodes
    global_node1.next = NULL;
    global_node1.locked = 0;
    global_node2.next = NULL;
    global_node2.locked = 0;

    // Create threads
    for (int i = 0; i < 2; i++){
        if (i == 1){
            usleep(150); // usleep takes microseconds, so multiply by 1000
        }
        pthread_create(&threads[i], NULL, thread_func, &i);
    }

    // Wait for threads to finish
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}