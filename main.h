#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <byteswap.h>
#include <pthread.h>

#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include <linux/icmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include <netinet/in_systm.h>

#define BUFFSIZE 1518

unsigned char buffer[BUFFSIZE];

int sockd;
int on;

/*GENERAL*/
int received = 0;

ssize_t max_size = 0; //DONE
ssize_t min_size = SSIZE_MAX; //DONE
int bytes_received = 0;

float   avg_size = 0; //DONE

/*DATA LINK LAYER*/
unsigned int arp_rep_count   = 0; //DONE
unsigned int arp_req_count   = 0; //DONE

float arp_rep_percent = 0; //DONE
float arp_req_percent = 0; //DONE

/*NETWORK LAYER*/
unsigned int icmp_count       = 0; //DONE
unsigned int icmp_req_count   = 0; //DONE
unsigned int icmp_rep_count   = 0; //DONE

float icmp_req_percent = 0;
float icmp_rep_percent = 0;

/*TRANSPORT LAYER*/
unsigned int tcp_count   = 0; //DONE
unsigned int udp_count   = 0; //DONE
unsigned int tcp_conns   = 0; //DONE
float tcp_percent = 0; //DONE
float udp_percent = 0; //DONE

unsigned char top_tcp_ports[5];
unsigned char top_udp_ports[5];

/*APPLICATION LAYER*/
unsigned int http_count    = 0; //DONE
unsigned int https_count   = 0; //DONE
unsigned int dns_count     = 0; //DONE

float http_percent  = 0;
float https_percent = 0;
float dns_percent   = 0;

unsigned char top_sites[5];


struct ifreq ifr;
struct ether_header* eth_header;

struct iphdr* ip_header;
struct arphdr* arp_header;
struct icmphdr* icmp_header;
struct tcphdr* tcp_header;
struct udphdr* udp_header;
