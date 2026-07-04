#include "player.h"

// NOTE: 65536 is the maximum number of values represented
//       by a two bit unsigned integer (uint16_t)
Player _players[65536];
uint16_t _player_id = 0;

void player_set_id(uint16_t id) { _player_id = id; }
Player* player_get_ptr() { return _players + _player_id; }

Player* player_create(ENetPeer *peer) {
	static uint16_t counter = 0;
	Player *player = _players + counter;
	player->id = counter++;
	player->active = true;
	player->peer = peer;
	return player;
}

void player_set_active(uint16_t id, bool active) {
	_players[id].active = active;
}

void player_set_pose(uint16_t id, const Vec3 *position, const Vec3 *rotation) {
	_players[id].position = *position;
	_players[id].rotation = *rotation;
}
