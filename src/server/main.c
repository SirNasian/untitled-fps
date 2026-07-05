#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION

#include <enet/enet.h>
#include <stb_image.h>

#include "../common/map.h"
#include "../common/math/vec3.h"
#include "../common/monster.h"
#include "../common/network.h"
#include "../common/player.h"
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

		Player *player = player_get_ptr_all();
		for (int i = 0; i < PLAYER_MAX_COUNT; i++) {
			if (!player[i].active) continue;
			for (int j = 0; j < MONSTER_MAX_COUNT; j++) {
				if (!monster_get_active(j)) continue;
				Vec3 v = vec3_sub(monster_get_position(j), player[i].position);
				float d2 = vec3_dot(v, v);

				if (d2 < 0.25) {
					monster_set_active(j, false);
					network_broadcast_monster_active(host, j);
				}

				if (d2 < 4) {
					Vec3 p0 = monster_get_position(j);
					Vec3 p1 = vec3_add(p0, vec3_mul(vec3_normalize(v), 0.015));
					if (!map_test_collide_circle(&map, p0.x, p1.z, 0.2)) p0.z = p1.z;
					if (!map_test_collide_circle(&map, p1.x, p0.z, 0.2)) p0.x = p1.x;
					monster_set_position(j, p0);
				}
			}
		}

		nanosleep(&(struct timespec){ 0, time_next_tick_ns(true) }, NULL);
	}

terminate:
	network_server_teardown(host);
	return 0;
}
