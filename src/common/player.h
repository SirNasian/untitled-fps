#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <enet/enet.h>

#include "math/vec3.h"

// NOTE: 65536 is the maximum number of values represented
//       by a two bit unsigned integer (uint16_t)
#define PLAYER_MAX_COUNT 65536

typedef struct {
	uint16_t id;
	bool active;
	Vec3 position;
	Vec3 rotation;
	ENetPeer *peer;
} Player;

void player_set_id(uint16_t);
Player* player_get_ptr();
Player* player_get_ptr_all();

Player* player_create(ENetPeer *peer);
void player_set_active(uint16_t id, bool active);
void player_set_pose(uint16_t id, const Vec3 *position, const Vec3 *rotation);
