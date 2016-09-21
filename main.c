#include "main.h"

void setup(char* options[]);
void receiver(void);
void arp_handler(void);
void ip_handler(void);
void icmp_handler(void);
void tcp_handler(void);
void udp_handler(void);
void payload_handler(unsigned char* data, int size);
void count_application_layer_protocol(unsigned int port_number);

float calculate_percentage(int counter);

int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		printf("Use %s <IF_NAME>\n", argv[0]);
		exit(1);
	}

	setup(argv);
	receiver();
}

void setup(char* options[])
{
	if((sockd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
	{
		printf("Erro na criação do socket.\n");
		exit(1);
	}

	strcpy(ifr.ifr_name, options[1]);

	if(ioctl(sockd, SIOCGIFINDEX, &ifr) < 0)
	{
		printf("Erro no ioctl!\n");
		exit(1);
	}

	eth_header  = (struct ether_header*) buffer;
	ip_header   = (struct iphdr*)   (buffer + sizeof(struct ether_header));
	arp_header  = (struct arphdr*)  (buffer + sizeof(struct ether_header));


	ioctl(sockd, SIOCGIFFLAGS, &ifr);
	ifr.ifr_flags |= IFF_PROMISC;
	ioctl(sockd, SIOCSIFFLAGS, &ifr);
}

void receiver(void)
{
	while (1)
	{
		ssize_t size = recv(sockd, buffer, BUFFSIZE, 0x0);

		received++;
		bytes_received += size;
		avg_size = bytes_received/received;

		if(size > max_size)
			max_size = size;
		if(size < min_size)
			min_size = size;
		u_int16_t ether_type = ntohs(eth_header->ether_type);
		if(ether_type == 0x0806)
			arp_handler();
		if(ether_type == 0x0800)
			ip_handler();
	}
}

void arp_handler()
{
	u_int16_t arp_operation = ntohs(arp_header->ar_op);
	if (arp_operation == 0x0001)
	{
		arp_req_count++;
		arp_req_percent = calculate_percentage(arp_rep_count);
	}

	else if (arp_operation == 0x0002)
	{
		arp_rep_count++;
		arp_rep_percent = calculate_percentage(arp_rep_count);
	}
}

void ip_handler()
{

	unsigned int ip_protocol = (unsigned int)ip_header->protocol;
	if (ip_protocol == 0x1)
		icmp_handler();

	else if (ip_protocol == 0x6)
		tcp_handler();

	else if (ip_protocol == 0x11)
		udp_handler();
}

void icmp_handler()
{
	icmp_header = (struct icmphdr*) (buffer + (sizeof(struct ether_header) + sizeof(struct iphdr)));
	unsigned int icmp_type = (unsigned int) icmp_header->type ;

	if(icmp_type == 0x0)
	{
		icmp_rep_count++;
		icmp_rep_percent = calculate_percentage(icmp_rep_count);
	}

	else if(icmp_type == 0x8)
	{
		icmp_req_count++;
		icmp_req_percent = calculate_percentage(icmp_req_count);
	}

	icmp_count++;
}

void tcp_handler()
{
	tcp_header  = (struct tcphdr*)  (buffer + (sizeof(struct ether_header) + sizeof(struct iphdr)));
	unsigned int tcp_dest_port   = (unsigned int) ntohs(tcp_header->dest);
	unsigned int ack = (unsigned int) tcp_header->ack;
	unsigned int syn = (unsigned int) tcp_header->syn;
	if(ack == 1 && syn == 0)
		++tcp_conns;

	count_application_layer_protocol(tcp_dest_port);

	tcp_count++;
	tcp_percent = calculate_percentage(tcp_count);

	/*int payload_size = BUFFSIZE - sizeof(struct ether_header) - (unsigned int)(ip_header->ihl)*4 - (unsigned int)tcp_header->doff*4;*/
	/*unsigned char* payload_data = (char *) (buffer + (sizeof(struct ether_header) + sizeof(struct iphdr)) + sizeof(struct tcphdr));*/
	/*payload_handler(payload_data, payload_size);*/
}

void udp_handler()
{
	udp_header  = (struct udphdr*)  (buffer + (sizeof(struct ether_header) + sizeof(struct iphdr)));
	unsigned int udp_dest_port   = (unsigned int) ntohs(udp_header->dest);

	count_application_layer_protocol(udp_dest_port);

	udp_count++;
	udp_percent = calculate_percentage(udp_count);
}

void payload_handler(unsigned char* data, int size)
{
	printf("size: %u\n", size);
	printf("data[0]: %c\n", data[0]);
}
void count_application_layer_protocol(unsigned int port_number)
{
	if(port_number == 80)
	{
		http_count++;
		http_percent = calculate_percentage(http_count);
	}
	else if(port_number == 53)
	{
		dns_count++;
		dns_percent = calculate_percentage(dns_count);
	}
	else if(port_number == 443)
	{
		https_count++;
		https_percent = calculate_percentage(http_count);
	}
}
float calculate_percentage(int counter)
{
	return (100 * counter) / (float)received;
}
