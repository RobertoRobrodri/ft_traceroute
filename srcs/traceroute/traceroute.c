#include "./../../includes/ft_traceroute.h"
/*
ICMP PACKET DETAILS
An IP header without options is 20 bytes.
An ICMP ECHO_REQUEST packet contains an additional 8 bytes worth of ICMP header
followed by an arbitrary amount of data. When a packetsize is given,
this indicated the size of this extra piece of data (the default is 56).
Thus the amount of data received inside of an IP packet of type ICMP ECHO_REPLY
will always be 8 bytes more than the requested data space (the ICMP header).
*/

int send_ping(int socket_fd, unsigned long host, double *start) {
	static int seq = 0;
	
	// destination
	struct sockaddr_in dst;
	memset(&dst, 0, sizeof(dst));

	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = host;

	// buff will be my packet
	unsigned char packet[PACKET_SIZE];
	// construct header
	struct icmphdr *icmp = (struct icmphdr *)(packet);
	// Payload
	memset(packet + sizeof(struct icmphdr), '1', PAYLOAD_SIZE);
	// icmp structure
	icmp->type = ICMP_ECHO; // Type 8
	icmp->code = 0;
	icmp->checksum = 0;
	icmp->un.echo.sequence = seq++;
	icmp->un.echo.id = getpid();
	// Calculate checksum from icmp header and payload
	icmp->checksum = calculate_checksum((unsigned short *)packet, PACKET_SIZE);
	// send packet
	*start = get_time_val();
	int status = sendto(socket_fd, packet, sizeof(packet), 0, (struct sockaddr *)&dst, sizeof(dst));
	if (status < 0) {
		perror("Error sending packet\n");
		return 1;
	}
	return 0;
}


/*I wish they allowed functions to FLUSH THE FUCKING STDOUT I could just use stderr but thats kinda bullshit*/
int recv_ping(int socket_fd, t_host_info *host_info, double *start, bool *success) {
	unsigned char buffer[sizeof(struct iphdr) + sizeof(struct icmphdr) + PAYLOAD_SIZE];
	struct sockaddr saddr;
	socklen_t saddr_len = sizeof(saddr);

	memset(buffer, 0, sizeof(buffer));
	int buflen = recvfrom(socket_fd, buffer, sizeof(buffer), 0, &saddr, &saddr_len);
	double end = get_time_val();
	struct iphdr *ip = (struct iphdr *)buffer;
	struct icmphdr *icmp = (struct icmphdr *)(buffer + ip->ihl * 4);
	char src_ip[INET_ADDRSTRLEN];

	if (buflen < 0) {
		printf("* ");
		return 0;
	}
	inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
	if (!(*success))
		printf("%s", src_ip);
	printf(" %.3fms", end - *start);
	*success = true;
	if (host_info->ip.s_addr == ip->saddr)
		return TRACE_SUCESS;
	return icmp->type;
}

void traceroute_loop(int socket_fd, t_host_info *host) {
	double start = 0;
	printf("traceroute to %s (%s): %d hops max\n", host->hostname, host->ip_str, TTL_MAX);
	for (int i = 1; i <= TTL_MAX; i++)
	{
		// Traceroute sends 3 probes by default for each TTL value
		printf("%d	", i);
		if (setsockopt(socket_fd, IPPROTO_IP, IP_TTL, &i, sizeof(i)) < 0) {
			perror("Error setting IP_TTL\n");
			exit(EXIT_FAILURE);
		};
		bool success = false;
		for (int j = 0; j < DEFAULT_PROBES; j++) {
			if (send_ping(socket_fd, host->ip.s_addr, &start) == 0) {
				int break_code = recv_ping(socket_fd, host, &start, &success);
				if (break_code == TRACE_SUCESS && j == 2)
					return ;
			}
		}
		printf("\n");
	}
}


unsigned short calculate_checksum(unsigned short *packet, size_t len) {
	unsigned short *buf = packet;
    unsigned int sum = 0;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
	return ~sum;
}

double	get_time_val(void)
{
	struct timeval	time;

	gettimeofday(&time, NULL);
    return (double)time.tv_sec * 1000.0 + (double)time.tv_usec / 1000.0;
}