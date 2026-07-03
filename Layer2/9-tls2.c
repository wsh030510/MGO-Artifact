//BUG：Semantic race between TCP Repair mode connection and TLS protocol upgrade.
/* 9-tls2.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#ifndef TCP_ULP
#define TCP_ULP 31
#endif
#ifndef TCP_REPAIR
#define TCP_REPAIR 19
#endif
#ifndef TCP_MD5SIG
#define TCP_MD5SIG 14
#endif

int sock_fd;

void *thread_repair_connect(void *arg) {
    int val = 1;
    setsockopt(sock_fd, SOL_TCP, TCP_REPAIR, &val, 4);
    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &addr.sin6_addr);
    connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));
    
    return NULL;
}

void *thread_tls_md5(void *arg) {
    usleep(1000);
    
    setsockopt(sock_fd, SOL_TCP, TCP_ULP, "tls", 4);
    struct {
        struct sockaddr_in6 tcpm_addr;
        uint8_t tcpm_keylen;
        uint8_t tcpm_key[80];
    } md5;
    memset(&md5, 0, sizeof(md5));
    md5.tcpm_addr.sin6_family = AF_INET6;
    inet_pton(AF_INET6, "::1", &md5.tcpm_addr.sin6_addr);
    md5.tcpm_keylen = 10;
    strcpy((char*)md5.tcpm_key, "password");
    
    setsockopt(sock_fd, SOL_TCP, TCP_MD5SIG, &md5, sizeof(md5));
    return NULL;
}

int main() {
    sock_fd = socket(AF_INET6, SOCK_STREAM, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_repair_connect, NULL);
    pthread_create(&t2, NULL, thread_tls_md5, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    close(sock_fd);
    return 0;
}