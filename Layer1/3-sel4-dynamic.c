// 81:52  60:83
// https://github.com/seL4/seL4/pull/199/commits/8ba22dcdc5fa667b41db6eb80b434 21a84868398
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

// CLH node
typedef struct clh_node {
    _Atomic bool locked;
} clh_node_t;

// CLH lock
typedef struct clh_lock {
    _Atomic(clh_node_t*) tail;
    pthread_key_t my_node_key;
} clh_lock_t;

// Initialize CLH lock
void clh_lock_init(clh_lock_t* lock) {
    clh_node_t* dummy = (clh_node_t*)malloc(sizeof(clh_node_t));
    atomic_store(&dummy->locked, false);
    atomic_store(&lock->tail, dummy);
    pthread_key_create(&lock->my_node_key, free);
}

// Destroy CLH lock
void clh_lock_destroy(clh_lock_t* lock) {
    free(atomic_load(&lock->tail));
    pthread_key_delete(lock->my_node_key);
}

// Acquire CLH lock
void clh_lock(clh_lock_t* lock) {
    clh_node_t* my_node = (clh_node_t*)malloc(sizeof(clh_node_t));
    atomic_store(&my_node->locked, true);
    pthread_setspecific(lock->my_node_key, my_node);

    clh_node_t* pred = atomic_exchange_explicit(&lock->tail, my_node, memory_order_acquire);

    while (atomic_load(&pred->locked)) {
        // Spin waiting
    }
}

// Release CLH lock
void clh_unlock(clh_lock_t* lock) {
    clh_node_t* my_node = (clh_node_t*)pthread_getspecific(lock->my_node_key);
    atomic_store(&my_node->locked, false);
}

typedef struct thread_arg {
    clh_lock_t* lock;
    int thread_num;
} thread_arg_t;

void* thread_func(void* arg) {
    thread_arg_t* thread_arg = (thread_arg_t*)arg;
    clh_lock_t* lock = thread_arg->lock;
    int thread_num = thread_arg->thread_num;
    clh_lock(lock);
    printf("Thread %lu acquired the lock\n", pthread_self());
    if (thread_num ==1) usleep(1000);
    clh_unlock(lock);
    printf("Thread %lu released the lock\n", pthread_self());
    return NULL;
}

int main() {
    clh_lock_t lock;
    clh_lock_init(&lock);
    thread_arg_t arg1 = { .lock = &lock, .thread_num = 1 };
    thread_arg_t arg2 = { .lock = &lock, .thread_num = 2 };
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, &arg1);
    pthread_create(&t2, NULL, thread_func, &arg2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    clh_lock_destroy(&lock);
    return 0;
}