#include "./../../includes/ft_traceroute.h"

void print_usage(void) {
	printf("Usage: traceroute [OPTION...] HOST ...\n");
	printf("Print the route packets trace to network host.\n\n");
	printf(" -f, --first-hop=NUM        set initial hop distance, i.e., time-to-live\n");
	printf(" -m, --max-hop=NUM          set maximal hop count (default: 64)\n");
	printf(" -q, --tries=NUM            send NUM probe packets per hop (default: 3)\n");
	printf(" -w, --wait=NUM             wait NUM seconds for response (default: 3)");
	printf(" --help                     give this help list\n");
}

bool str_isdigit(char *str) {
	while (*str)
	{
		if (isdigit(*str) == 0) {
			return false;
		}
		str++;
	}
	return true ;
}