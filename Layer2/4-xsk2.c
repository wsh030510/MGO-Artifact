/*
# commit 42fddcc7c64b723a867c7b2f5f7505e244212f13 
# xsk->state = XSK_BOUND in xsk_bind()
*/
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_xdp.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>
#ifndef AF_XDP
#define AF_XDP 44
#endif
#ifndef SOL_XDP
#define SOL_XDP 283
#endif

int xdp_fd, inet_fd;

void *thread_setup_ring(void *arg) {
    int size = 8;
    setsockopt(xdp_fd, SOL_XDP, XDP_RX_RING, &size, sizeof(size));
    int size2 = 32;
    setsockopt(xdp_fd, SOL_XDP, XDP_UMEM_FILL_RING, &size2, sizeof(size2));
    struct sockaddr_xdp sxdp;
    memset(&sxdp, 0, sizeof(sxdp));
    sxdp.sxdp_family = AF_XDP;
    sxdp.sxdp_ifindex = 0; 
    bind(xdp_fd, (struct sockaddr *)&sxdp, sizeof(sxdp));
   
    return NULL;
}

void *thread_recv(void *arg) {
    struct mmsghdr msgs[1];
    memset(msgs, 0, sizeof(msgs));
    recvmmsg(xdp_fd, msgs, 1, 0, NULL);
    return NULL;
}

int main() {
    xdp_fd = socket(AF_XDP, SOCK_RAW, 0);
    inet_fd = socket(AF_INET6, SOCK_DGRAM, 0);
    struct xdp_umem_reg mr;
    memset(&mr, 0, sizeof(mr));
    mr.addr = (unsigned long)malloc(4096);
    mr.len = 4096;
    mr.chunk_size = 2048;
    setsockopt(xdp_fd, SOL_XDP, XDP_UMEM_REG, &mr, sizeof(mr));

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_setup_ring, NULL);
    pthread_create(&t2, NULL, thread_recv, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}