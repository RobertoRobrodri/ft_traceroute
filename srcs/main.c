#include "./../includes/ft_traceroute.h"

static int ft_traceroute(t_host_info *host) {
	int socket_fd;
	struct timeval timeout = {TIMEOUT, 0};

	// Create raw socket
	if ((socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		dprintf(STDERR_FILENO, "Socket error: %s", strerror(errno));
		return 1;
	}
	if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
		dprintf(STDERR_FILENO, "Setsockopt error: %s", strerror(errno));
		return 1;
	};
	traceroute_loop(socket_fd, host);
	close(socket_fd);
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
	while (++i < argc)
	{
		if (argv[i][0] == '-' && argv[i][1])
		{
			if (strcmp(argv[i], "--help") == 0) {
				print_usage();
				exit(EXIT_SUCCESS);
			}
			else {
				dprintf(STDERR_FILENO, "traceroute: invalid option -- '%s'\n", argv[i]);
				dprintf(STDERR_FILENO, "Try 'traceroute --help' for more information.\n");
				exit(EXIT_FAILURE);
			}
		}
		host = argv[i];
	}
	t_host_info *host_info = dns_look_up(host);
	ft_traceroute(host_info);
	free(host_info->hostname);
	free(host_info);
	printf("\n");
	return EXIT_SUCCESS;
}