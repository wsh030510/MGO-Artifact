#include <pthread.h>

int close(int fd) { return 0; }
int fcntl(int fd, int cmd, ...) { return 0; }

int socks[3] = {3, 4, 5};

void *thread_race_close(void *arg) {
    close(socks[0]); 
    return NULL;
}

void *thread_race_fcntl(void *arg) {
    fcntl(socks[0], 1); 
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_race_close, NULL);
    pthread_create(&t2, NULL, thread_race_fcntl, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}