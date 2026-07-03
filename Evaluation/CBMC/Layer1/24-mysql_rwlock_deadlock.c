#include <pthread.h>
#include <assert.h>

int lock_recursive = 0;
int lock_writer_thread = 0;

void *thread_writer(void *arg) {
    lock_writer_thread = 1001;
    lock_recursive = 1;
    return NULL;
}

void *thread_reader(void *arg) {
    int r_recursive = lock_recursive;
    int r_writer = lock_writer_thread;
    
    if (r_recursive == 1) {
        assert(r_writer == 1001);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}