#include <pthread.h>
#include <assert.h>
#include <stddef.h>

typedef struct rte_mcslock {
    struct rte_mcslock *next;
    int locked;
} rte_mcslock_t;

rte_mcslock_t* global_lock = NULL;
int cs = 0;

void rte_mcslock_lock(rte_mcslock_t **msl, rte_mcslock_t *me) {
    me->locked = 1;
    me->next = NULL;
    rte_mcslock_t *prev = __sync_lock_test_and_set(msl, me);
    if (prev == NULL) return;
    prev->next = me; 
    __CPROVER_assume(me->locked == 0);
}

void rte_mcslock_unlock(rte_mcslock_t **msl, rte_mcslock_t *me) {
    if (me->next == NULL) {
        rte_mcslock_t *save_me = me;
        if (__sync_bool_compare_and_swap(msl, &save_me, NULL)) return;
        __CPROVER_assume(me->next != NULL);
    }
    me->next->locked = 0; 
}

void* thread_func(void* arg) {
    rte_mcslock_t node;
    rte_mcslock_lock(&global_lock, &node);
    cs++;
    assert(cs == 1);
    cs--;
    rte_mcslock_unlock(&global_lock, &node);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_func, NULL);
    pthread_create(&t2, NULL, thread_func, NULL);
    return 0;
}