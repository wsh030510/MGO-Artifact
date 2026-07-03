#include <pthread.h>
#include <assert.h>

int lock_word = 0;
int writer_thread = 0;
int recursive = 0;

void* thread_writer(void* arg) {
    writer_thread = 1;
    lock_word = 1; 
    recursive = 1; 
    return NULL;
}

void* thread_reader(void* arg) {
    if (writer_thread == 1) {
        assert(recursive == 1); 
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_writer, NULL);
    pthread_create(&t2, NULL, thread_reader, NULL);
    return 0;
}