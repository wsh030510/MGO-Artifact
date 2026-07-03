#include <pthread.h>
#include <assert.h>

int db_data = 0;
int db_flag = 0;

void *writer(void *arg) {
    db_data = 42;
    __sync_synchronize(); 
    db_flag = 1;
    return NULL;
}

void *reader(void *arg) {
    int f = db_flag;
    int d = db_data;
    if (f == 1) {
        assert(d == 42);
    }
    return NULL;
}

int main() {
    pthread_t w, r;
    pthread_create(&w, NULL, writer, NULL);
    pthread_create(&r, NULL, reader, NULL);
    pthread_join(w, NULL);
    pthread_join(r, NULL);
    return 0;
}