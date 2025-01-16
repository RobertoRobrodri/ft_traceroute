#include "./../../includes/ft_traceroute.h"

/*HAHAHA im gonna use the bonus to flush the STDOUT JUST LIKE IT SHOULD BE*/
int recv_packet(int socket_fd, t_host_info *host_info, double *start, bool *success) {
	unsigned char buffer[sizeof(struct iphdr) + sizeof(struct icmphdr) + PAYLOAD_SIZE];
	struct sockaddr saddr;
	socklen_t saddr_len = sizeof(saddr);

	memset(buffer, 0, sizeof(buffer));
	int buflen = recvfrom(socket_fd, buffer, sizeof(buffer), 0, &saddr, &saddr_len);
	double end = get_time_val();
	struct iphdr *ip = (struct iphdr *)buffer;
	struct icmphdr *icmp = (struct icmphdr *)(buffer + ip->ihl * 4);

	if (buflen < 0) {
		printf("* ");
		fflush(stdout);
		return 0;
	}
	if (!(*success)) {
		char src_ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(ip->saddr), src_ip, INET_ADDRSTRLEN);
		printf("%s", src_ip);
	}
	printf(" %.3fms", end - *start);
	*success = true;
	return host_info->ip.s_addr == ip->saddr ? TRACE_SUCCESS : icmp->type;
}

void traceroute_loop(int icmp_socket, int udp_socket, t_host_info *host, t_trace_vars opt_args, send_packet_func send_packet) {
	double start = 0;

	printf("traceroute to %s (%s): %d hops max\n", host->hostname, host->ip_str, opt_args.ttl_max);
	int i = opt_args.first_hop, seq = 1; 
	do {
		printf("%d	", seq);
		bool success = false;
		int send_socket = opt_args.icmp ? icmp_socket : udp_socket;
		for (int j = 0; j < opt_args.probes; j++) {
			if (setsockopt(send_socket, IPPROTO_IP, IP_TTL, &i, sizeof(i)) < 0) {
				dprintf(STDERR_FILENO, "Setsockopt error: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			};
			if (send_packet(send_socket, host->ip.s_addr, &start, opt_args.port + seq) == 0) {
				if ((recv_packet(icmp_socket, host, &start, &success) == TRACE_SUCCESS) && j == opt_args.probes - 1) {
					printf("\n");
					return ;
				}
			}
		}
		printf("\n");
		i++;
		seq++;
	} while (seq <= opt_args.ttl_max) ;
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