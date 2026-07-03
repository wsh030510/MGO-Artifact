#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct clh_node {
    bool locked;
} clh_node_t;

typedef struct clh_lock {
    clh_node_t* tail;
} clh_lock_t;

clh_lock_t global_lock;
clh_node_t dummy_node = {false};
int cs = 0;

void clh_lock_init(clh_lock_t* lock) {
    lock->tail = &dummy_node;
}

void clh_lock(clh_lock_t* lock, clh_node_t* my_node) {
    my_node->locked = true;
    clh_node_t* pred = __sync_lock_test_and_set(&lock->tail, my_node);
    __CPROVER_assume(!pred->locked);
}

void clh_unlock(clh_node_t* my_node) {
    my_node->locked = false; 
}

void* thread_func(void* arg) {
    clh_node_t node;
    clh_lock(&global_lock, &node);
    cs++;
    assert(cs == 1);
    cs--;
    clh_unlock(&node);
    return NULL;
}

int main() {
    clh_lock_init(&global_lock);
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    return 0;
}