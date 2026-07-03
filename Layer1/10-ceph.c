//https://github.com/ceph/ceph/pull/38765

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
#include <unistd.h>

typedef struct {
    unsigned int nlock;
    pthread_t locked_by;
    int recursive;
} CustomLock;
CustomLock lock = {0, 0, 0}; // Initialize lock

int is_locked_by_me() {
    return lock.nlock > 0 && pthread_equal(lock.locked_by, pthread_self());
    //return nlock.load(std::memory_order_acquire) > 0 && locked_by == std::this_thread::get_id();
}

void _post_lock() {
    if (!lock.recursive)
        assert(lock.nlock == 0);
    printf("1111");
    lock.locked_by = pthread_self();
    lock.nlock++;
}

// Thread function specifically for calling _post_lock
void* lock_thread(void* arg) {
    _post_lock();
    printf("Lock acquired in lock_thread.\n");
    return NULL;
}

// Thread function specifically for calling is_locked_by_me
void* check_lock_thread(void* arg) {
    if (is_locked_by_me()) {
        printf("Lock is held by check_lock_thread, which should not happen.\n");
    } else {
        printf("Lock is not held by check_lock_thread.\n");
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create a thread to attempt locking
    if(pthread_create(&t1, NULL, lock_thread, &lock)) {
        fprintf(stderr, "Error creating lock_thread\n");
        return 1;
    }

    // Create another thread to check lock state
    if(pthread_create(&t2, NULL, check_lock_thread, &lock)) {
        fprintf(stderr, "Error creating check_lock_thread\n");
        return 1;
    }

    // Wait for first thread to complete, ensuring lock has been acquired
    pthread_join(t1, NULL);
    // Wait for second thread to complete
    pthread_join(t2, NULL);

    return 0;
}