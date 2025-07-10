#include "vport.h"
#include "sysutils.h"

#include "tap_utils.h"
#include <arpa/inet.h>
#include <assert.h>
#include <net/ethernet.h>
#include <pthread.h>
#include <stdint.h>
/* Vport Instance */
struct vport_t {
  int tapfd;
  int vport_sockfd;                // socket for connecting with
  struct sockaddr_in vswitch_addr; // Vswitch address
} void vport_init(struct vport_t *vport, const char *server_ip,
                  int server_port);
void *forward_ether_to_switch(void *raw_vport);
void *forward_ether_to_tap(void *raw_vport);

int main(int argc, char **argv) {
  if (argc != 3) {
    ERROR_PRINT_THEN_EXIT("Usage: <server_ip> <server_port>\n");
  }
  const char *server_ip = argv[1];
  int server_port = atoi(argv[2]);
  struct vport_t vport;

  vport_init(&vport, server_ip, server_port);
  pthread_t up_forwarder;
}
void vport_init(struct vport_t *vport, const char *ip_address, int ip_port) {
  char ifname[IFNAMSIZ] = "tapyuan";
  int tapfd = tap_alloc(ifname);
  if (tapfd < 0) {
    ERROR_PRINT_THEN_EXIT("Failed to create tap socket:%s\n", strerror(errno));
  }
  int vport_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (vport_sockfd < 0) {
    ERROR_PRINT_THEN_EXIT("Failed to create a UDP socket %s\n",
                          strerror(errno));
  }
  struct sockaddr_in vswitch_addr;
  memset(&vswitch_addr, 0, sizeof(vswitch_addr));
  vswitch_addr.sin_family = AF_INET;
  vswitch_addr.sin_port = htons(ip_port);
  if ((inet_pton(AF_INET, server_ip_str, (void *)&vswitch_addr.sin_addr)) !=
      1) {
    ERROR_PRINT_THEN_EXIT(
        "There was an error in converting the ip to the binary:%s\n",
        strerror(errno));
  }
  vport->tapfd = tapfd;
  vport->vswitch_addr = vswitch_addr;
  vport->vport_sockfd = vport_sockfd;
  printf("TAP Device Name:%s.Virtual Switch address:%s Port:%d", ifname,
         ip_address, ip_port);
}
void *forward_ether_to_switch(void *raw_vport) {
  struct vport_t *vport = (struct vport_t *)raw_vport;
  char ether_data[ETHER_MAX_LEN];
  while (true) {
    int ether_datasz = read(vport->tapfd, ether_data, sizeof(ether_data));
    if (ether_datasz > 0) {
      assert(ether_datasz >= 14);
      const struct ether_header *hdr = (const struct ether_header *)ether_data;
      ssize_t sendsz = sendto(vport->vport_sockfd, ether_data, ether_datasz, 0,
                              (struct sockadrr *)&vport->vswitch_addr,
                              sizeof(vport->vswitch_addr));
      if (sendsz != ether_datasz) {
        fprintf(stderr, "There was a mismatch in the data sizes of sent and "
                        "received data\n");
      }
    }
    printf("[VPort] Sent to VSwitch:"
           " dhost<%02x:%02x:%02x:%02x:%02x:%02x>"
           " shost<%02x:%02x:%02x:%02x:%02x:%02x>"
           " type<%04x>"
           " datasz=<%d>\n",
           hdr->ether_dhost[0], hdr->ether_dhost[1], hdr->ether_dhost[2],
           hdr->ether_dhost[3], hdr->ether_dhost[4], hdr->ether_dhost[5],
           hdr->ether_shost[0], hdr->ether_shost[1], hdr->ether_shost[2],
           hdr->ether_shost[3], hdr->ether_shost[4], hdr->ether_shost[5],
           ntohs(hdr->ether_type), ether_datasz);
  }
}

void *
