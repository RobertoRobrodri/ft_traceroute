#include "./../includes/ft_traceroute.h"

static int ft_traceroute(t_host_info *host, t_trace_vars opt_args) {
	int icmp_socket, udp_socket;
	struct timeval timeout = {opt_args.timeout, 0};

	// Create ICMP socket to receive (also send if -I option)
	if ((icmp_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		dprintf(STDERR_FILENO, "Socket error: %s", strerror(errno));
		return 1;
	}
	// Create UDP socket to send (Ignored if -I option)
	if ((udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		dprintf(STDERR_FILENO, "Socket error: %s", strerror(errno));
		return 1;
	}
	if (setsockopt(icmp_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		dprintf(STDERR_FILENO, "Setsockopt error: %s", strerror(errno));
		return 1;
	};
	traceroute_loop(icmp_socket, udp_socket, host, opt_args);
	close(icmp_socket);
	close(udp_socket);
	return 0;
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
	while (++i < argc)
	{
		if (argv[i][0] == '-' && argv[i][1])
		{
			if (strcmp(argv[i], "--help") == 0) {
				print_usage();
				exit(EXIT_SUCCESS);
			}
			else if (strcmp(argv[i], "-f") == 0 | strcmp(argv[i], "--first-hop") == 0) {
				if (++i == argc) {
					dprintf(STDERR_FILENO, "traceroute: option requires an argument -- '%s'\n", argv[--i]);
					dprintf(STDERR_FILENO, "Try 'traceroute --help' or 'traceroute --usage' for more information.\n");
					return EXIT_FAILURE;
				}
				if (str_isdigit(argv[i]) == 0 | (opt_args.first_hop = atoi(argv[i])) <= 0 | opt_args.first_hop > 255) {
					dprintf(STDERR_FILENO, "traceroute: impossible distance '%s'\n", argv[i]);
					return EXIT_FAILURE;
				}
			}
			else if (strcmp(argv[i], "-m") == 0 | strcmp(argv[i], "--max-hop") == 0) {
				if (++i == argc) {
					dprintf(STDERR_FILENO, "traceroute: option requires an argument -- '%s'\n", argv[--i]);
					dprintf(STDERR_FILENO, "Try 'traceroute --help' or 'traceroute --usage' for more information.\n");
					return EXIT_FAILURE;
				}
				if (str_isdigit(argv[i]) == 0 | (opt_args.ttl_max = atoi(argv[i])) <= 0 | opt_args.ttl_max > 255) {
					dprintf(STDERR_FILENO, "traceroute: invalid hops value '%s'\n", argv[i]);
					return EXIT_FAILURE;
				}
			}
			else if (strcmp(argv[i], "-q") == 0 | strcmp(argv[i], "--tries") == 0) {
				if (++i == argc) {
					dprintf(STDERR_FILENO, "traceroute: option requires an argument -- '%s'\n", argv[--i]);
					dprintf(STDERR_FILENO, "Try 'traceroute --help' or 'traceroute --usage' for more information.\n");
					return EXIT_FAILURE;
				}
				if (str_isdigit(argv[i]) == 0 | (opt_args.probes = atoi(argv[i])) <= 0 | opt_args.probes > 10) {
					dprintf(STDERR_FILENO, "traceroute: number of tries should be between 1 and 10\n");
					return EXIT_FAILURE;
				}
			}
			else if (strcmp(argv[i], "-w") == 0 | strcmp(argv[i], "--wait") == 0) {
				if (++i == argc) {
					dprintf(STDERR_FILENO, "traceroute: option requires an argument -- '%s'\n", argv[--i]);
					dprintf(STDERR_FILENO, "Try 'traceroute --help' or 'traceroute --usage' for more information.\n");
					return EXIT_FAILURE;
				}
				if (str_isdigit(argv[i]) == 0 | (opt_args.timeout = atoi(argv[i])) <= 0 | opt_args.timeout > 60) {
					dprintf(STDERR_FILENO, "traceroute: ridiculous waiting time `%s'\n", argv[i]);
					return EXIT_FAILURE;
				}
			}
			else if (strcmp(argv[i], "-p") == 0 | strcmp(argv[i], "--port") == 0) {
				if (++i == argc) {
					dprintf(STDERR_FILENO, "traceroute: option requires an argument -- '%s'\n", argv[--i]);
					dprintf(STDERR_FILENO, "Try 'traceroute --help' or 'traceroute --usage' for more information.\n");
					return EXIT_FAILURE;
				}
				if (str_isdigit(argv[i]) == 0 | (opt_args.port = atoi(argv[i])) <= 0 | opt_args.port > 65535) {
					dprintf(STDERR_FILENO, "traceroute: invalid port number `%s'\n", argv[i]);
					return EXIT_FAILURE;
				}
			}
			else if (strcmp(argv[i], "-I") == 0)
				opt_args.icmp = true;
			else {
				dprintf(STDERR_FILENO, "traceroute: invalid option -- '%s'\n", argv[i]);
				dprintf(STDERR_FILENO, "Try 'traceroute --help' for more information.\n");
				exit(EXIT_FAILURE);
			}
		}
		host = argv[i];
	}
	t_host_info *host_info = dns_look_up(host);
	ft_traceroute(host_info, opt_args);
	free(host_info->hostname);
	free(host_info);
	return EXIT_SUCCESS;
}