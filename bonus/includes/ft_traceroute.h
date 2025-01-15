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
#include <ctype.h>
#include <netinet/udp.h>

#define PAYLOAD_SIZE 56
#define PACKET_SIZE sizeof(struct icmphdr) + PAYLOAD_SIZE
#define TRACE_SUCCESS 42

#define DEFAULT_PROBES 3
#define TTL_MAX 64
#define FIRST_HOP 1
#define TIMEOUT 5
#define DEFAULT_PORT 33434

typedef struct s_host_info {
	char *hostname;
	char ip_str[INET_ADDRSTRLEN];
	struct in_addr ip;
} t_host_info;

typedef struct s_trace_vars {
	int probes;
	int ttl_max;
	int first_hop;
	int timeout;
	int port;
	
	bool icmp;
} t_trace_vars;

void traceroute_loop(int socket_fd, t_host_info *host, t_trace_vars opt_args);
int send_packet_icmp(int socket_fd, unsigned long host, double *start);
int send_packet_udp(int socket_fd, unsigned long host, double *start, int port);
int recv_packet(int socket_fd, t_host_info *host_info, double *start, bool *success);

unsigned short calculate_checksum(unsigned short *packet, size_t len);

t_host_info *dns_look_up(char *host);

double	get_time_val(void);

void print_usage(void);
bool str_isdigit(char *str);