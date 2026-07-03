//BUG：State race between bind() and sendmsg() on an RDS socket.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#ifndef AF_RDS
#define AF_RDS 21
#endif
#ifndef SOL_RDS
#define SOL_RDS 276
#endif

int rds_sock;

void *thread_bind(void *arg) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(4000);
    bind(rds_sock, (struct sockaddr*)&addr, sizeof(addr));
    return NULL;
}

void *thread_send(void *arg) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(4001);

    struct msghdr msg;
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = &addr;
    msg.msg_namelen = sizeof(addr);
    sendmsg(rds_sock, &msg, 0);
    usleep(100);
    close(rds_sock);
    return NULL;
}

int main() {
    rds_sock = socket(AF_RDS, SOCK_SEQPACKET, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bind, NULL);
    pthread_create(&t2, NULL, thread_send, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}