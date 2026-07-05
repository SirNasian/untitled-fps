#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "math/vec3.h"

// NOTE: 65536 is the maximum number of values represented
//       by a two bit unsigned integer (uint16_t)
#define MONSTER_MAX_COUNT 65536

typedef struct {
	bool active;
	Vec3 position;
} Monster;

uint16_t monster_create(Vec3 position);
uint16_t monster_get_active_count();

void monster_set_active(uint16_t id, bool active);
void monster_set_position(uint16_t id, Vec3 position);

bool monster_get_active(uint16_t id);
Vec3 monster_get_position(uint16_t id);
