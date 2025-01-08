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


#define TTL_MAX 64
#define TIMEOUT 5
#define PAYLOAD_SIZE 56
#define PACKET_SIZE sizeof(struct icmphdr) + PAYLOAD_SIZE
#define TRACE_SUCESS 42
#define DEFAULT_PROBES 3

typedef struct s_list {
    void *data;
    struct s_list *next;
} t_list;

typedef struct s_host_info {
	char *hostname;
	char ip_str[INET_ADDRSTRLEN];
	struct in_addr ip;
} t_host_info;

void traceroute_loop(int socket_fd, t_host_info *host);
int send_ping(int socket_fd, unsigned long host, double *start);
int recv_ping(int socket_fd, t_host_info *host_info, double *start, bool *success);

unsigned short calculate_checksum(unsigned short *packet, size_t len);

t_host_info *dns_look_up(char *host);

double	get_time_val(void);

t_list *lst_new(void *data);
void lst_add_back(t_list **lst, t_list *new);
void free_list(t_list **lst);
void free_list_data(t_list **lst);

void print_usage(void);