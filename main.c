#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if_arp.h>

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
int arp_rep_count   = 0;
int arp_req_count   = 0;

/*NETWORK LAYER*/
int icmp_count = 0;
int icmp_req_count = 0;
int icmp_rep_count = 0;

/*TRANSPORT LAYER*/
int tcp_count = 0;
int udp_count = 0;
int tcp_conns = 0;
unsigned char top_tcp_ports[5];
unsigned char top_udp_ports[5];

/*APPLICATION LAYER*/
int http_count = 0;
int dns_count  = 0;
unsigned char top_sites[5];


struct ifreq ifr;
struct ether_header* eth_header;

struct iphdr* ip_header;
struct arphdr* arp_header;

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("Use %s <IF_NAME>\n", argv[0]);
		exit(1);
	}
	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		printf("Erro na criação do socket.\n");
		exit(1);
	}

	strcpy(ifr.ifr_name, argv[1]);

	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
	{
		printf("Erro no ioctl!\n");
		exit(1);
	}

	eth_header = (struct ether_header *) buffer;
	ip_header  = (struct iphdr*) (buffer + sizeof(struct ether_header));
	arp_header  = (struct arphdr*) (buffer + sizeof(struct ether_header));
	/*ip_header  = (struct iphdr*) (buffer + sizeof(struct ether_header));*/

	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);

	while (1) {
		ssize_t size = recv(sockd, buffer, BUFFSIZE, 0x0);

		++received;
		bytes_received += size;
		avg_size = bytes_received/received;

		if(size > max_size)
			max_size = size;
		if(size < min_size)
			min_size = size;
		if(eth_header->ether_type == 0x0608)
			printf("arp %d\n", arp_header->ar_op);
		if(eth_header->ether_type == 0x0008)
			printf("ip %d\n", ip_header->protocol);
		/*printf("arp: %d\n",arp_header->arp_op);*/
		/*else if(eth_header->ether_type == ETHERTYPE_IP)*/
			/*printf("IP: %d\n",eth_header->ether_type);*/
	}
}
