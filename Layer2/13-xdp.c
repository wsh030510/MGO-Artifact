//# BUG:Race between XDP rx-polling (xsk_poll) and BPF tracepoint loading.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/syscall.h>
#include <linux/bpf.h>
#include <pthread.h>
#include <string.h>

int tun_fd, xdp_fd;

void *thread_bpf(void *arg) {
    union bpf_attr attr = {
        .prog_type = BPF_PROG_TYPE_RAW_TRACEPOINT,
        .insns = (unsigned long)0, // Dummy
        .insn_cnt = 0,
        .license = (unsigned long)"GPL",
    };
    syscall(__NR_bpf, BPF_PROG_LOAD, &attr, sizeof(attr));
    union bpf_attr attr_open = {
        .raw_tracepoint.name = (unsigned long)"kfree",
        .raw_tracepoint.prog_fd = -1,
    };
    syscall(__NR_bpf, BPF_RAW_TRACEPOINT_OPEN, &attr_open, sizeof(attr_open));
    return NULL;
}

void *thread_poll(void *arg) {
    int size = 2;
    setsockopt(xdp_fd, 283, 2, &size, 4); 
    struct pollfd fds[2];
    fds[0].fd = tun_fd; fds[0].events = POLLIN;
    fds[1].fd = xdp_fd; fds[1].events = POLLIN;
    poll(fds, 2, 1);
    return NULL;
}

int main() {
    tun_fd = open("/dev/net/tun", O_RDWR);
    xdp_fd = socket(44, SOCK_RAW, 0); 

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_poll, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}