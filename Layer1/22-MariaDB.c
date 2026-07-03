/*
 https://bugs.mysql.com/bug.php?id=76135
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

/* Simulate database low-level spinlock — version lacking full memory barrier */
typedef struct {
    int lock;
} spinlock_t;

static inline void spin_lock(spinlock_t *l) {
    /* __sync_lock_test_and_set implies full barrier on x86,
       but on ARM it only implements LDREX/STREX without automatic DMB,
       thus cannot guarantee subsequent stores are not reordered before lock acquisition. */
    while (__sync_lock_test_and_set(&l->lock, 1)) {
        /* busy-wait */
    }
}

static inline void spin_unlock(spinlock_t *l) {
    /* __sync_lock_release implies release barrier on x86,
       but on ARM it is just an ordinary store without DMB, and cannot guarantee
       all previous stores are visible to other CPUs before lock release. */
    __sync_lock_release(&l->lock);
}

/* Shared data */
spinlock_t mutex = {0};
int data = 0;
int flag = 0;

/* Stop when bug is detected */
volatile int stop = 0;

void *writer(void *arg) {
    while (!stop) {
        spin_lock(&mutex);
        /* Critical section: write data first, then set flag */
        data = 42;
        flag = 1;
        spin_unlock(&mutex);
        usleep(1);  /* Give reader a chance */
    }
    return NULL;
}

void *reader(void *arg) {
    while (!stop) {
        spin_lock(&mutex);
        int f = flag;
        int d = data;
        spin_unlock(&mutex);

        /* Consistency check: if flag is 1, data must be 42 */
        if (f == 1 && d != 42) {
            printf("BUG: flag=1 but data=%d (expected 42)\n", d);
            stop = 1;
            assert(0);  /* Trigger crash to simulate database crash */
        }
        usleep(1);
    }
    return NULL;
}

int main() {
    pthread_t w, r;
    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r, NULL, reader, NULL);

    /* Let the test run for a while, or until bug triggers */
    sleep(5);
    stop = 1;

    pthread_join(w, NULL);
    pthread_join(r, NULL);
    printf("Test finished (no bug detected on this run).\n");
    return 0;
}