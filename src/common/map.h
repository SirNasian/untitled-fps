#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "math/vec3.h"

typedef struct {
	uint32_t width;
	uint32_t height;
	uint8_t *data;
} MapData;

void map_load_data(const char *path, MapData *map);
static inline size_t map_get_size(const MapData *map) { return map->width * map->height; }

Vec3 map_get_player_spawn(const MapData *map);

bool map_test_collide_circle(const MapData *map, float cx, float cz, float cr);
bool map_test_wall(const MapData *map, int x, int z);
