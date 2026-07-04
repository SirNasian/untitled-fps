#include <signal.h>
#include <stdbool.h>
#include <time.h>

#include <enet/enet.h>

#include "../common/network.h"
#include "../common/time.h"

int exit_code = EXIT_SUCCESS;
bool running = true;

void handle_interrupt(int _) {
	if (!running)
		exit(EXIT_FAILURE);
	running = false;
}

int main() {
	signal(SIGINT, handle_interrupt);

	ENetHost *host;
	if (!network_server_setup(NULL, 42069, &host))
		goto terminate;

	while (running) {
		if (time_next_tick_ns(false) == 0)
			network_server_service(host);
		nanosleep(&(struct timespec){ 0, time_next_tick_ns(true) }, NULL);
	}

terminate:
	enet_host_destroy(host);
	enet_deinitialize();
	return 0;
}
