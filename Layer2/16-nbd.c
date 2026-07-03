/*
 * commit c2da049f419417808466c529999170f5c3ef7d3d
 * Note: Simulates the race between NBD Netlink configuration and device opening.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <linux/nbd.h>
#include <string.h>
#include <pthread.h>

int nl_sock;

// Thread 0: Simulate NBD Connect via Netlink
void *thread_netlink(void *arg) {
    struct sockaddr_nl sa;
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;

    char buf[128] = {0}; 
    struct iovec iov = { buf, sizeof(buf) };
    struct msghdr msg = {
        .msg_name = &sa,
        .msg_namelen = sizeof(sa),
        .msg_iov = &iov,
        .msg_iovlen = 1
    };

    for(int i=0; i<10; i++) {
        sendmsg(nl_sock, &msg, 0);
        usleep(100);
    }
    return NULL;
}

// Thread 1: Open NBD Device
void *thread_open_dev(void *arg) {
    int fd = open("/dev/nbd0", O_RDWR);
    if (fd < 0) {

    } else {
        close(fd);
    }
    return NULL;
}

int main() {
    nl_sock = socket(AF_NETLINK, SOCK_RAW, 16);
    if (nl_sock < 0) {
        perror("socket netlink");
    }

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_netlink, NULL);
    pthread_create(&t2, NULL, thread_open_dev, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    
    if (nl_sock >= 0) close(nl_sock);
    return 0;
}