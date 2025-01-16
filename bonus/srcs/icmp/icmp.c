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

int send_packet_icmp(int socket_fd, unsigned long host, double *start, int port) {
	(void)port;
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
	if (sendto(socket_fd, packet, sizeof(packet), 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
		dprintf(STDERR_FILENO, "Sendto error: %s\n", strerror(errno));
		return 1;
	}
	return 0;
}