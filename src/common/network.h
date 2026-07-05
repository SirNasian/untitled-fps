#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <enet/enet.h>

#include "../common/map.h"

typedef enum {
	NETWORK_PACKET_TYPE_NONE,
	NETWORK_PACKET_TYPE_PLAYER_CONNECT,
	NETWORK_PACKET_TYPE_PLAYER_DISCONNECT,
	NETWORK_PACKET_TYPE_PLAYER_ID,
	NETWORK_PACKET_TYPE_PLAYER_POSE,
	NETWORK_PACKET_TYPE_MAP_DATA,
	NETWORK_PACKET_TYPE_MONSTER_LIST,
	NETWORK_PACKET_TYPE_MONSTER_ACTIVE,
	NETWORK_PACKET_TYPE_MONSTER_POSE,
	NETWORK_PACKET_TYPE_LAST,
} NetworkPacketType;

typedef enum {
	NETWORK_CHANNEL_EVENTS,
	NETWORK_CHANNEL_UPDATES,
} NetworkChannel;

bool network_server_setup(const char *listen_address, int listen_port, ENetHost **host);
void network_server_teardown(ENetHost *host);
void network_server_service(ENetHost *host, const MapData *map);

bool network_client_setup(const char *server_address, int server_port, ENetHost **host, ENetPeer **server, MapData *map_data);
void network_client_teardown(ENetHost *host);
void network_client_service(ENetHost *host, ENetPeer *server);

void network_broadcast_monster_active(ENetHost *host, uint16_t id);
