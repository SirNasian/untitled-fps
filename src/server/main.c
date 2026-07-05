#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION

#include <enet/enet.h>
#include <stb_image.h>

#include "../common/map.h"
#include "../common/network.h"
#include "../common/time.h"

static int exit_code = EXIT_SUCCESS;
static bool running = true;

static MapData map;

void handle_interrupt(int _) {
	if (!running)
		exit(EXIT_FAILURE);
	running = false;
}

int main(int argc, const char **argv) {
	signal(SIGINT, handle_interrupt);

	map_load_data(argc > 1 ? argv[1] : "assets/map.png", &map);
	if (!map.data) goto terminate;

	map_load_monsters(&map);

	const char *listen_address = argc > 2 ? argv[2] : NULL;
	int listen_port = argc > 3 ? atoi(argv[3]) : 42069;

	ENetHost *host;
	if (!network_server_setup(listen_address, listen_port, &host))
		goto terminate;

	printf("listening on %s:%d\n", listen_address ? listen_address : "0.0.0.0", listen_port);

	while (running) {
		if (time_next_tick_ns(false) == 0)
			network_server_service(host, &map);
		nanosleep(&(struct timespec){ 0, time_next_tick_ns(true) }, NULL);
	}

terminate:
	network_server_teardown(host);
	return 0;
}
