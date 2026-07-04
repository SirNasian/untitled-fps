#ifndef COMMON_NETWORK_H
#define COMMON_NETWORK_H

#include <stdbool.h>

#include <enet/enet.h>

typedef enum {
	NETWORK_PACKET_TYPE_NONE,
	NETWORK_PACKET_TYPE_PLAYER_CONNECT,
	NETWORK_PACKET_TYPE_PLAYER_DISCONNECT,
	NETWORK_PACKET_TYPE_PLAYER_ID,
	NETWORK_PACKET_TYPE_PLAYER_POSE,
	NETWORK_PACKET_TYPE_LAST,
} NetworkPacketType;

typedef enum {
	NETWORK_CHANNEL_EVENTS,
	NETWORK_CHANNEL_UPDATES,
} NetworkChannel;

bool network_server_setup(const char *listen_address, int listen_port, ENetHost **host);
void network_server_service(ENetHost *host);

bool network_client_setup(const char *server_address, int server_port, ENetHost **host, ENetPeer **server);
void network_client_service(ENetHost *host, ENetPeer *server);

#endif
