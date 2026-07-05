#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <enet/enet.h>

#include "../common/network.h"
#include "../common/time.h"

static int exit_code = EXIT_SUCCESS;
static bool running = true;

static uint8_t map_data[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,
	1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,
	1,0,0,0,0,1,1,0,0,1,1,0,0,0,0,1,
	1,0,1,0,0,0,0,0,0,0,0,0,0,1,0,1,
	1,0,1,1,0,0,0,0,0,0,0,0,1,1,0,1,
	1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
};

void handle_interrupt(int _) {
	if (!running)
		exit(EXIT_FAILURE);
	running = false;
}

int main(int argc, const char **argv) {
	signal(SIGINT, handle_interrupt);

	const char *listen_address = argc > 1 ? argv[1] : NULL;
	int listen_port = argc > 2 ? atoi(argv[2]) : 42069;

	ENetHost *host;
	if (!network_server_setup(listen_address, listen_port, &host))
		goto terminate;

	printf("listening on %s:%d\n", listen_address ? listen_address : "0.0.0.0", listen_port);

	while (running) {
		if (time_next_tick_ns(false) == 0)
			network_server_service(host, map_data);
		nanosleep(&(struct timespec){ 0, time_next_tick_ns(true) }, NULL);
	}

terminate:
	network_server_teardown(host);
	return 0;
}
