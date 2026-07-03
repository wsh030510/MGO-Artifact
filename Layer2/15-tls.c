//# BUG: Teardown race between async I/O submission and tls_getsockopt.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/syscall.h>
#include <linux/aio_abi.h>
#include <pthread.h>
#include <string.h>

int tcp_sock;
aio_context_t ctx = 0;

void *thread_io(void *arg) {
    syscall(__NR_io_setup, 2, &ctx);
    int c1 = socket(AF_NETLINK, SOCK_RAW, 21); 
    struct iocb cb;
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes = c1;
    cb.aio_lio_opcode = IOCB_CMD_PWRITE;
    struct iocb *cbs[1] = { &cb };
    syscall(__NR_io_submit, ctx, 1, cbs);
    struct iovec iov = { "data", 4 };
    writev(c1, &iov, 1);
    return NULL;
}

void *thread_tls(void *arg) {
    setsockopt(tcp_sock, SOL_TCP, 31, "tls", 4); // 31=TCP_ULP
    return NULL;
}

int main() {
    tcp_sock = socket(AF_INET6, SOCK_STREAM, 0);
    
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_io, NULL);
    pthread_create(&t2, NULL, thread_tls, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}