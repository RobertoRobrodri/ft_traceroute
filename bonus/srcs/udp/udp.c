#include "./../../includes/ft_traceroute.h"

int send_packet_udp(int socket_fd, unsigned long host, double *start, int port) {
	// destination
	struct sockaddr_in dst;
	memset(&dst, 0, sizeof(dst));

	dst.sin_family = AF_INET;
	dst.sin_addr.s_addr = host;
    dst.sin_port = htons(port);

	// buff will be my packet
	unsigned char packet[sizeof(struct udphdr) + PAYLOAD_SIZE];
	// construct header
	struct udphdr *udp = (struct udphdr *)(packet);
	// Payload
	memset(packet + sizeof(struct udphdr), '1', PAYLOAD_SIZE);
	// icmp structure
    udp->source = htons(2048);
    udp->dest = dst.sin_port;
    udp->len = htons(sizeof(struct udphdr) + PAYLOAD_SIZE);
    udp->check = 0;
	// Calculate checksum from icmp header and payload
	udp->check = calculate_checksum((unsigned short *)packet, PACKET_SIZE);
	// send packet
	*start = get_time_val();
	if (sendto(socket_fd, packet, sizeof(packet), 0, (struct sockaddr *)&dst, sizeof(dst)) < 0) {
		dprintf(STDERR_FILENO, "Sendto error: %s\n", strerror(errno));
		return 1;
	}
	return 0;
}