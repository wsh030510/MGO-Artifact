//BUG：Store-Store reordering between TCP option setting and SMC data transmission.
/* 10-smc2.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <pthread.h>

#ifndef AF_SMC
#define AF_SMC 43
#endif

int sock_fd;

void *thread_opt(void *arg) {
    int val = 1;
    setsockopt(sock_fd, 6, 0x22, &val, 4);
    return NULL;
}

void *thread_send(void *arg) {
    struct mmsghdr msgs[1];
    struct iovec iov;
    char buf[10] = "99";
    memset(&msgs, 0, sizeof(msgs));
    iov.iov_base = buf;
    iov.iov_len = 2;
    msgs[0].msg_hdr.msg_iov = &iov;
    msgs[0].msg_hdr.msg_iovlen = 1;
    
    sendmmsg(sock_fd, msgs, 1, 0);
    return NULL;
}

int main() {
    sock_fd = socket(AF_SMC, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        // perror("socket AF_SMC");
    }

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_opt, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    if (sock_fd >= 0) close(sock_fd);
    return 0;
}