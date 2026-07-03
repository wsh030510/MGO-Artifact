#include <pthread.h>
#include <assert.h>
#include <stdint.h>

uint16_t tag = 0;
uint32_t size = 0;
int global_hdr_ready = 0;

void* writer_thread(void* arg) {
    tag = 1;
    size = 1024;
    global_hdr_ready = 1;
    return NULL;
}

void* reader_thread(void* arg) {
    if (global_hdr_ready == 1) {
        assert(tag == 1 && size == 1024);
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