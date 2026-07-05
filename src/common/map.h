#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "math/vec3.h"

#define MAP_DATA_WIDTH 16
#define MAP_DATA_SIZE (MAP_DATA_WIDTH * MAP_DATA_WIDTH)

Vec3 map_get_player_spawn(const uint8_t *map);

bool map_test_collide_circle(const uint8_t *map, float cx, float cz, float cr);
bool map_test_wall(const uint8_t *map, int x, int z);
