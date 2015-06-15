#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#if INET_ADDRSTRLEN > INET6_ADDRSTRLEN
#define BUF_LEN #INET_ADDRSTRLEN
#else
#define BUF_LEN INET6_ADDRSTRLEN
#endif

int main() {
    if (access("/proc/net", R_OK)) {
        goto errorout;
    }
    if (access("/proc/net/if_inet6", R_OK)) {
        goto errorout;
    }
    int ipv4_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (ipv4_fd == -1) {
        goto errorout;
    }

    struct ifconf stuff;
    stuff.ifc_len = 0;
    stuff.ifc_buf = NULL;

    if (ioctl(ipv4_fd, SIOCGIFCONF, &stuff)) {
        goto errorout;
    }
    if (!(stuff.ifc_buf = malloc(stuff.ifc_len))) {
        goto errorout;
    }
    if (ioctl(ipv4_fd, SIOCGIFCONF, &stuff)) {
        goto errorout;
    }

    int i, len;
    char addr[BUF_LEN];

    for (i = 0, len = stuff.ifc_len / sizeof(struct ifreq); i < len; ++i) {
        if (!strncmp(stuff.ifc_req[i].ifr_name, "lo", 2)) { continue; }
        struct sockaddr_in *sockaddr =  (struct sockaddr_in*) &stuff.ifc_req[i].ifr_addr;
        printf("%s\t%s\n", inet_ntop(AF_INET, &sockaddr->sin_addr, addr, BUF_LEN), stuff.ifc_req[i].ifr_name);
    }

    return 0;

errorout:
    perror("something went wrong");
    exit(EXIT_FAILURE);
}
