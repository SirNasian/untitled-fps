#include "map.h"

#include <math.h>

#include <stb_image.h>

#define MAP_VALUE_NONE    0
#define MAP_VALUE_WALL    1
#define MAP_VALUE_PLAYER  2
#define MAP_VALUE_MONSTER 3

#define MAP_PIXEL_WALL    (Pixel){ 0x00, 0x00, 0x00 }
#define MAP_PIXEL_PLAYER  (Pixel){ 0x00, 0xFF, 0x00 }
#define MAP_PIXEL_MONSTER (Pixel){ 0xFF, 0xFF, 0x00 }

typedef struct {
	stbi_uc r, g, b;
} Pixel;

bool pixel_equal(Pixel a, Pixel b) { return a.r == b.r && a.g == b.g && a.b == b.b; }

void map_load_data(const char *path, MapData *map) {
	int width, height, channels;
	stbi_uc *image = stbi_load(path, &width, &height, &channels, 3);
	if (!image) return;

	map->width  = width;
	map->height = height;
	map->data   = malloc(map_get_size(map));
	for (int i = 0; i < map_get_size(map); i++) {
		Pixel *p = (Pixel*)(image+i*3);
		if      (pixel_equal(*p, MAP_PIXEL_WALL))    map->data[i] = MAP_VALUE_WALL;
		else if (pixel_equal(*p, MAP_PIXEL_PLAYER))  map->data[i] = MAP_VALUE_PLAYER;
		else if (pixel_equal(*p, MAP_PIXEL_MONSTER)) map->data[i] = MAP_VALUE_MONSTER;
		else                                         map->data[i] = MAP_VALUE_NONE;
	}

	stbi_image_free(image);
}

Vec3 map_get_player_spawn(const MapData *map) {
	for (int i = 0; i < map_get_size(map); i++)
		if (map->data[i] == MAP_VALUE_PLAYER)
			return (Vec3){
				(i % map->width) + 0.5,
				0,
				(truncf((float)i / map->width)) + 0.5
			};
	return (Vec3){ 0, 0, 0 };
}

bool map_test_wall(const MapData *map, int x, int z) {
	return map->data[map->width * z + x] == MAP_VALUE_WALL;
}

bool map_test_collide_circle(const MapData *map, float cx, float cz, float cr) {
	int tx_min = (int)floorf(cx - cr);
	int tx_max = (int)floorf(cx + cr);
	int tz_min = (int)floorf(cz - cr);
	int tz_max = (int)floorf(cz + cr);

	for (int tz = tz_min; tz <= tz_max; tz++) {
		for (int tx = tx_min; tx <= tx_max; tx++) {
			if (tx >= 0 && tx < map->width && tz >= 0 && tz < map->height && (map->data[map->width*tz + tx] == MAP_VALUE_WALL)) {
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
