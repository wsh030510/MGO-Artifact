/*
 * BUG:Protocol upgrade race between TLS configuration (setsockopt) and concurrent socket I/O (write).
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/epoll.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

#ifndef TCP_ULP
#define TCP_ULP 31
#endif
#ifndef SOL_TLS
#define SOL_TLS 282
#endif
#ifndef TLS_TX
#define TLS_TX 1
#endif

int sock_fd;
int epoll_fd;

// Thread 0: Enable and Configure TLS
void *thread_tls_config(void *arg) {
    usleep(1000); 
    setsockopt(sock_fd, SOL_TCP, TCP_ULP, "tls", 4);
    char key_material[60] = {0}; 
    setsockopt(sock_fd, SOL_TLS, TLS_TX, key_material, 56);
    
    return NULL;
}

// Thread 1: Write and Epoll
void *thread_io(void *arg) {
    write(sock_fd, "test_data", 9);
    
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT;
    ev.data.fd = sock_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock_fd, &ev);
    
    return NULL;
}

int main() {
    sock_fd = socket(AF_INET6, SOCK_STREAM, 0);
    
    epoll_fd = epoll_create(1);

    struct sockaddr_in6 addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(443); 
    inet_pton(AF_INET6, "::1", &addr.sin6_addr);
    connect(sock_fd, (struct sockaddr*)&addr, sizeof(addr));

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_tls_config, NULL);
    pthread_create(&t2, NULL, thread_io, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    close(sock_fd);
    close(epoll_fd);
    return 0;
}