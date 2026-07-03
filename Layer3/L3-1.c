/*BUG:Protocol upgrade race between SMC socket transition (setsockopt) and data transmission (sendmmsg).*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <pthread.h>

#ifndef TCP_ULP
#define TCP_ULP 31
#endif

int sock_fd;

void *thread_ulp(void *arg) {
    setsockopt(sock_fd, SOL_TCP, TCP_ULP, "smc", 4);
    return NULL;
}

void *thread_send(void *arg) {
    // sendmmsg
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
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_ulp, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    close(sock_fd);
    return 0;
}