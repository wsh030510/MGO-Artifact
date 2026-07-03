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
int s_igmp, s_kcm;

void *thread_config(void *arg) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "syzkaller1");
    
    // ioctl$sock_SIOCSIFVLAN_ADD_VLAN_CMD
    ioctl(s_igmp, 0x8983, &ifr);
    return NULL;
}

void *thread_transmit(void *arg) {
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strcpy(ifr.ifr_name, "syzkaller1");
    ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;
    
    ioctl(s_kcm, SIOCSIFHWADDR, &ifr);

    char buf[128] = {0};
    write(tun_fd, buf, sizeof(buf));
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

    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread_config, NULL);
    pthread_create(&t2, NULL, thread_transmit, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}