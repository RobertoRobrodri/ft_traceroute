#include "./../includes/ft_traceroute.h"

static int ft_traceroute(t_host_info *host, t_trace_vars opt_args) {
	int icmp_socket, udp_socket = -1;
	struct timeval timeout = {opt_args.timeout, 0};
	send_packet_func send_packet;

	// Create ICMP socket to receive (also send if -I option)
	if ((icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		dprintf(STDERR_FILENO, "Socket error: %s", strerror(errno));
		return 1;
	}
	// Create UDP socket to send (Ignored if -I option)
	if (opt_args.icmp)
		send_packet = send_packet_icmp;
	else {
		if ((udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
			dprintf(STDERR_FILENO, "Socket error: %s", strerror(errno));
			return 1;
		}
		send_packet = send_packet_udp;
	}
	if (setsockopt(icmp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		dprintf(STDERR_FILENO, "Setsockopt error: %s", strerror(errno));
		return 1;
	};
	traceroute_loop(icmp_socket, udp_socket, host, opt_args, send_packet);
	opt_args.icmp == true ? close(icmp_socket) : close(icmp_socket) && close(udp_socket);
	return 0;
}

static int parse_int_argument(const char *arg, int min, int max, const char *error_message) {
    char *endptr;
    long value = strtol(arg, &endptr, 10);
    if (*endptr != '\0' || value < min || value > max) {
        handle_error(error_message, arg);
    }
    return (int)value;
}

static void parse_arguments(int argc, char **argv, t_trace_vars *opt_args, char **host) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1])
		{
            if (strcmp(argv[i], "--help") == 0) {
                print_usage();
                exit(EXIT_SUCCESS);
            }
			else if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--first-hop") == 0) {
                if (++i == argc)
					handle_error("traceroute: option requires an argument -- '%s'\n", argv[i - 1]);
                opt_args->first_hop = parse_int_argument(argv[i], 1, 255, "traceroute: impossible distance '%s'\n");
            }
			else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max-hop") == 0) {
                if (++i == argc)
					handle_error("traceroute: option requires an argument -- '%s'\n", argv[i - 1]);
                opt_args->ttl_max = parse_int_argument(argv[i], 1, 255, "traceroute: invalid hops value '%s'\n");
            }
			else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--tries") == 0) {
                if (++i == argc)
					handle_error("traceroute: option requires an argument -- '%s'\n", argv[i - 1]);
                opt_args->probes = parse_int_argument(argv[i], 1, 10, "traceroute: number of tries should be between 1 and 10\n");
            }
			else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--wait") == 0) {
                if (++i == argc)
					handle_error("traceroute: option requires an argument -- '%s'\n", argv[i - 1]);
                opt_args->timeout = parse_int_argument(argv[i], 1, 60, "traceroute: ridiculous waiting time '%s'\n");
            }
			else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
                if (++i == argc)
					handle_error("traceroute: option requires an argument -- '%s'\n", argv[i - 1]);
                opt_args->port = parse_int_argument(argv[i], 1, 65535, "traceroute: invalid port number '%s'\n");
            }
			else if (strcmp(argv[i], "-I") == 0) {
                opt_args->icmp = true;
            }
			else
                handle_error("traceroute: invalid option -- '%s'\n", argv[i]);
        }
		else
			*host = argv[i];
    }
}

int main(int argc, char **argv) {
	if (argc < 2) {
		dprintf(STDERR_FILENO, "traceroute: missing host operand\n");
		dprintf(STDERR_FILENO, "Try 'traceroute --help' for more information.\n");
		return 1;
	}
	if (getuid() != 0) {
		dprintf(STDERR_FILENO, "Require root\n");
		return 1;
	}
	int i = 0;
	char *host = NULL;
	t_trace_vars opt_args = {DEFAULT_PROBES, TTL_MAX, FIRST_HOP, TIMEOUT, DEFAULT_PORT, false};
	parse_arguments(argc, argv, &opt_args, &host);
	t_host_info *host_info = dns_look_up(host);
	ft_traceroute(host_info, opt_args);
	free(host_info->hostname);
	free(host_info);
	return EXIT_SUCCESS;
}