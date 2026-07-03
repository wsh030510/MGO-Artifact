//# commit 7ee47dcfff1835ff75a794d1075b6b5f5462cfed
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <pthread.h>

int socks[3];

void *thread_race_close(void *arg) {
    close(socks[0]); 
    return NULL;
}

void *thread_race_fcntl(void *arg) {
    fcntl(socks[0], F_GETFD);
    return NULL;
}

int main() {
    socks[0] = socket(29, SOCK_DGRAM, 6);
    socks[1] = socket(29, SOCK_DGRAM, 6);
    socks[2] = socket(29, SOCK_DGRAM, 6);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_race_close, NULL);
    pthread_create(&t2, NULL, thread_race_fcntl, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}