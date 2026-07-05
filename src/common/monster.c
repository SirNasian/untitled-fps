#include "monster.h"

Monster _monsters[MONSTER_MAX_COUNT];

uint16_t monster_create(Vec3 position) {
	static uint16_t id = 0;
	_monsters[id].active = true;
	_monsters[id].position = position;
	return id++;
}

uint16_t monster_get_active_count() {
	int c = 0;
	for (int i = 0; i < MONSTER_MAX_COUNT; i++)
		if (_monsters[i].active)
			c++;
	return c;
}

void monster_set_active(uint16_t id, bool active) {
	_monsters[id].active = active;
}

void monster_set_position(uint16_t id, Vec3 position) {
	_monsters[id].position = position;
}

bool monster_get_active(uint16_t id) {
	return _monsters[id].active;
}

Vec3 monster_get_position(uint16_t id) {
	return _monsters[id].position;
}
