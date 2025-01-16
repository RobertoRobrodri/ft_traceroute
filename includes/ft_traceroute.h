#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <limits.h>
#include <math.h>
#include <sys/time.h>
#include <errno.h>


#define TTL_MAX 64
#define TIMEOUT 5
#define PAYLOAD_SIZE 56
#define PACKET_SIZE sizeof(struct icmphdr) + PAYLOAD_SIZE
#define TRACE_SUCCESS 42
#define DEFAULT_PROBES 3

typedef struct s_host_info {
	char *hostname;
	char ip_str[INET_ADDRSTRLEN];
	struct in_addr ip;
} t_host_info;

void traceroute_loop(int socket_fd, t_host_info *host);
int send_packet(int socket_fd, unsigned long host, double *start);
int recv_packet(int socket_fd, t_host_info *host_info, double *start, bool *success);

unsigned short calculate_checksum(unsigned short *packet, size_t len);

t_host_info *dns_look_up(char *host);

double	get_time_val(void);

void print_usage(void);