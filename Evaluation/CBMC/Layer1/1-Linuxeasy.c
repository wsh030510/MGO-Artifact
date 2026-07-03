#include <pthread.h>
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct mcs_node {
    bool locked;
    struct mcs_node* next;
} mcs_node_t;

typedef struct {
    mcs_node_t* tail;
} qspinlock_t;

qspinlock_t my_lock = {NULL};
int cs = 0;

void qspin_lock(qspinlock_t* lock, mcs_node_t* mynode) {
    mynode->next = NULL;
    mynode->locked = true;
    mcs_node_t* prev = __sync_lock_test_and_set(&lock->tail, mynode);
    if (prev != NULL) {
        prev->next = mynode;
        __CPROVER_assume(!mynode->locked);
    }
}

void qspin_unlock(qspinlock_t* lock, mcs_node_t* mynode) {
    if (!mynode->next) {
        if (__sync_bool_compare_and_swap(&lock->tail, mynode, NULL)) {
            return;
        }
        __CPROVER_assume(mynode->next != NULL);
    }
    mynode->next->locked = false;
}

void* thread_func(void *arg) {
    mcs_node_t mynode;
    qspin_lock(&my_lock, &mynode);
    cs++;
    assert(cs == 1);
    cs--;
    qspin_unlock(&my_lock, &mynode);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}