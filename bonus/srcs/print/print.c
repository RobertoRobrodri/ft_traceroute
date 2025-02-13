#include "./../../includes/ft_traceroute.h"

void print_usage(void) {
	printf("Usage: traceroute [OPTION...] HOST ...\n");
	printf("Print the route packets trace to network host.\n\n");
	printf(" -f, --first-hop=NUM        set initial hop distance, i.e., time-to-live\n");
	printf(" -I, --icmp                 use ICMP ECHO as probe\n");
	printf(" -p, --port=PORT            use destination PORT port (default: 33434)");
	printf(" -m, --max-hop=NUM          set maximal hop count (default: 64)\n");
	printf(" -q, --tries=NUM            send NUM probe packets per hop (default: 3)\n");
	printf(" -w, --wait=NUM             wait NUM seconds for response (default: 3)");
	printf(" --help                     give this help list\n");
}

void handle_error(const char *message, const char *arg) {
    dprintf(STDERR_FILENO, message, arg);
    dprintf(STDERR_FILENO, "Try 'traceroute --help' for more information.\n");
    exit(EXIT_FAILURE);
}