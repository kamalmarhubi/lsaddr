#include <argp.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#if INET_ADDRSTRLEN > INET6_ADDRSTRLEN
#define ADDRSTRLEN #INET_ADDRSTRLEN
#else
#define ADDRSTRLEN INET6_ADDRSTRLEN
#endif

/* Options without short options */
#define OPT_INCLUDE_LOOPBACK 1
#define OPT_INCLUDE_LINK_LOCAL 2

static struct argp_option options[] = {
    {"ipv4", '4', 0, 0, "List IPv4 addresses", 0},
    {"ipv6", '6', 0, 0, "List IPv6 addresses", 0},
    {"include-loopback", OPT_INCLUDE_LOOPBACK, 0, 0,
     "Include addresses for the loopback interface", 0},
    {"include-link-local", OPT_INCLUDE_LINK_LOCAL, 0, 0,
     "Include IPv6 link-local addresses", 0},
    {0}};

/* Command line options */
struct args {
  /* The union is a little hack to allow setting either ipv4 or ipv6 to result
   * in ip_version_specified being true. */
  union {
    int ip_version_specified;
    struct {
      int ipv4 : 1;
      int ipv6 : 1;
    };
  };
  int include_loopback;
  int include_link_local;
  char **interfaces;
  int num_interfaces;
};

static error_t parse_opt(int key, char *arg __attribute__((unused)),
                         struct argp_state *state) {
  struct args *args = state->input;

  switch (key) {
    case '4':
      args->ipv4 = 1;
      break;
    case '6':
      args->ipv6 = 1;
      break;
    case ARGP_KEY_ARGS:
      args->interfaces = state->argv + state->next;
      args->num_interfaces = state->argc - state->next;
      break;
    default:
      return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, 0, 0, 0, 0, 0};

int main(int argc, char **argv) {
  struct args args = {
      .ip_version_specified = 0, /* also sets ipv4 and ipv6 to 0 */
      .include_loopback = 0,
      .include_link_local = 0,
      .interfaces = NULL,
      .num_interfaces = 0,
  };

  argp_parse(&argp, argc, argv, 0, 0, &args);

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

  char addr[ADDRSTRLEN];

  for (int i = 0, len = stuff.ifc_len / sizeof(struct ifreq); i < len; ++i) {
    struct sockaddr_in *sockaddr =
        (struct sockaddr_in *)&stuff.ifc_req[i].ifr_addr;
    printf("%s\t%s\n",
           inet_ntop(AF_INET, &sockaddr->sin_addr, addr, sizeof(addr)),
           stuff.ifc_req[i].ifr_name);
  }

  return 0;

errorout:
  perror("something went wrong");
  exit(EXIT_FAILURE);
}
