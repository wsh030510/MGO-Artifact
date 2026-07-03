/*
commit_hash:c1102e9d49eb36c0be18cb3e16f6e46ffb717964
*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <string.h>
#include <pthread.h>
#include <net/if_arp.h>

int tun_fd;
int s_kcm, s_igmp;

// Thread 0: Add VLAN
void *thread_vlan(void *arg) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "syzkaller1"); 
    ioctl(s_igmp, 0x8983, &ifr);
    return NULL;
}

// Thread 1: Change HW Addr & Write
void *thread_hw_write(void *arg) {
    // ioctl$SIOCSIFHWADDR
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "syzkaller1");
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    memcpy(ifr.ifr_hwaddr.sa_data, "\x00\xaa\xbb\xcc\xdd\xee", 6);
    
    ioctl(s_kcm, SIOCSIFHWADDR, &ifr);
    char data[100] = {0};
    write(tun_fd, data, sizeof(data));
    return NULL;
}

int main() {
    tun_fd = open("/dev/net/tun", O_RDWR);
    struct ifreq ifr_tun;
    memset(&ifr_tun, 0, sizeof(ifr_tun));
    strcpy(ifr_tun.ifr_name, "syzkaller1");
    ifr_tun.ifr_flags = IFF_TAP | IFF_NO_PI;
    ioctl(tun_fd, TUNSETIFF, &ifr_tun);
    s_kcm = socket(AF_INET, SOCK_DGRAM, 0); 
    s_igmp = socket(AF_INET6, SOCK_DGRAM, 0);

    pthread_t t0, t1;
    pthread_create(&t0, NULL, thread_vlan, NULL);
    pthread_create(&t1, NULL, thread_hw_write, NULL);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);
    return 0;
}