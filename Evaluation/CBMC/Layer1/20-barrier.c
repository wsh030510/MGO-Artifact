#include <pthread.h>
#include <assert.h>

int data = 0; 
int flag = 0; 

void* writer_thread(void *args) {
    data = 42; 
    flag = 1;
    return NULL;
}

void* reader_thread(void *args) {
    if (flag == 1) {
        assert(data == 42);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, writer_thread, NULL);
    pthread_create(&t2, NULL, reader_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}