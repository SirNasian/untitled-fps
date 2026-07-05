#include "map.h"

#include <math.h>

#define MAP_VALUE_WALL 1
#define MAP_VALUE_PLAYER 2
#define MAP_VALUE_MONSTER 3

Vec3 map_get_player_spawn(const uint8_t *map) {
	for (int i = 0; i < MAP_DATA_SIZE; i++)
		if (map[i] == MAP_VALUE_PLAYER)
			return (Vec3){
				(i % MAP_DATA_WIDTH) + 0.5,
				0,
				(truncf((float)i / MAP_DATA_WIDTH)) + 0.5
			};
	return (Vec3){ 0, 0, 0 };
}

bool map_test_wall(const uint8_t *map, int x, int z) {
	return map[MAP_DATA_WIDTH * z + x] == MAP_VALUE_WALL;
}

bool map_test_collide_circle(const uint8_t *map, float cx, float cz, float cr) {
	int tx_min = (int)floorf(cx - cr);
	int tx_max = (int)floorf(cx + cr);
	int tz_min = (int)floorf(cz - cr);
	int tz_max = (int)floorf(cz + cr);

	for (int tz = tz_min; tz <= tz_max; tz++) {
		for (int tx = tx_min; tx <= tx_max; tx++) {
			if (tx >= 0 && tx < 16 && tz >= 0 && tz < 16 && (map[16*tz + tx] == MAP_VALUE_WALL)) {
				float nx = fmaxf(tx, fminf(cx, tx + 1.0f));
				float nz = fmaxf(tz, fminf(cz, tz + 1.0f));
				float dx = cx - nx;
				float dz = cz - nz;
				if (dx*dx + dz*dz < cr*cr)
					return true;
			}
		}
	}

	return false;
}
