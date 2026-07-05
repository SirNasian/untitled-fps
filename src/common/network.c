#include <stdio.h>
#include <string.h>

#include "network.h"
#include "map.h"
#include "player.h"

void network_broadcast_player_connect(ENetHost *host, const Player *player) {
	uint8_t data[3] = { NETWORK_PACKET_TYPE_PLAYER_CONNECT };
	memcpy(data+1, &player->id, 2);
	ENetPacket *packet = enet_packet_create(data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, NETWORK_CHANNEL_EVENTS, packet);
}

void network_send_player_connect(ENetPeer *peer, const Player *player) {
	uint8_t data[3] = { NETWORK_PACKET_TYPE_PLAYER_CONNECT };
	memcpy(data+1, &player->id, 2);
	ENetPacket *packet = enet_packet_create(data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, NETWORK_CHANNEL_EVENTS, packet);
}

void network_receive_player_connect(const ENetEvent *event) {
	if (event->packet->data[0] != NETWORK_PACKET_TYPE_PLAYER_CONNECT) return;
	uint16_t player_id;
	memcpy(&player_id, event->packet->data+1, 2);
	player_set_active(player_id, true);
}

void network_broadcast_player_disconnect(ENetHost *host, const Player *player) {
	uint8_t data[3] = { NETWORK_PACKET_TYPE_PLAYER_DISCONNECT };
	memcpy(data+1, &player->id, 2);
	ENetPacket *packet = enet_packet_create(data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(host, NETWORK_CHANNEL_EVENTS, packet);
}

void network_receive_player_disconnect(const ENetEvent *event) {
	if (event->packet->data[0] != NETWORK_PACKET_TYPE_PLAYER_DISCONNECT) return;
	uint16_t player_id;
	memcpy(&player_id, event->packet->data+1, 2);
	player_set_active(player_id, false);
}

void network_send_player_id(ENetPeer *peer, uint16_t player_id) {
	uint8_t data[3] = { NETWORK_PACKET_TYPE_PLAYER_ID };
	memcpy(data+1, &player_id, 2);
	ENetPacket *packet = enet_packet_create(data, sizeof(data), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, NETWORK_CHANNEL_EVENTS, packet);
}

void network_receive_player_id(const ENetEvent *event) {
	if (event->packet->data[0] != NETWORK_PACKET_TYPE_PLAYER_ID) return;
	uint16_t player_id;
	memcpy(&player_id, event->packet->data+1, 2);
	player_set_id(player_id);
}

ENetPacket* network_packet_player_pose_create(const Player *player) {
	uint8_t data[1 + 2 + 2*sizeof(Vec3)] = { NETWORK_PACKET_TYPE_PLAYER_POSE };
	memcpy(data+01, &player->id, 2);
	memcpy(data+03, &player->position, sizeof(Vec3));
	memcpy(data+15, &player->rotation, sizeof(Vec3));
	return enet_packet_create(data, sizeof(data), 0);
}

void network_packet_player_pose_read(const ENetPacket *packet, uint16_t *player_id, Vec3 *position, Vec3 *rotation) {
	if (packet->data[0] != NETWORK_PACKET_TYPE_PLAYER_POSE) return;
	memcpy(player_id, packet->data+01, 2);
	memcpy(position,  packet->data+03, sizeof(Vec3));
	memcpy(rotation,  packet->data+15, sizeof(Vec3));
}

void network_broadcast_player_pose(ENetHost *host, const Player *player, ENetPeer *exclude) {
	ENetPacket *packet = network_packet_player_pose_create(player);
	for (size_t i = 0; i < host->peerCount; i++) {
		if (&host->peers[i] == exclude) continue;
		if (host->peers[i].state != ENET_PEER_STATE_CONNECTED) continue;
		enet_peer_send(&host->peers[i], NETWORK_CHANNEL_UPDATES, packet);
	}
}

void network_send_player_pose(ENetPeer *peer, const Player *player) {
	enet_peer_send(
		peer,
		NETWORK_CHANNEL_UPDATES,
		network_packet_player_pose_create(player)
	);
}

void network_receive_player_pose(const ENetEvent *event) {
	uint16_t player_id;
	Vec3 position, rotation;

	// NOTE: only server uses peer->data to store player information
	//       and the server should trust its own player id assignments
	//       over client-sent ids from the network
	network_packet_player_pose_read(event->packet, &player_id, &position, &rotation);
	if (event->peer->data != NULL)
		player_id = ((Player*)event->peer->data)->id;

	player_set_pose(player_id, &position, &rotation);
}

// NOTE: the map is expected to be a 16x16 array
void network_send_map_data(ENetPeer *peer, const uint8_t *map) {
	uint8_t data[1+MAP_DATA_SIZE] = { NETWORK_PACKET_TYPE_MAP_DATA };
	memcpy(data+1, map, MAP_DATA_SIZE);
	ENetPacket *packet = enet_packet_create(data, 1+MAP_DATA_SIZE, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(peer, NETWORK_CHANNEL_EVENTS, packet);
}

void network_receive_map_data(ENetEvent *event, uint8_t *map) {
	if (event->packet->data[0] != NETWORK_PACKET_TYPE_MAP_DATA) return;
	memcpy(map, event->packet->data+1, MAP_DATA_SIZE);
}

void network_client_receive(const ENetEvent *event) {
	if (event->type != ENET_EVENT_TYPE_RECEIVE) return;
	switch ((NetworkPacketType)event->packet->data[0]) {
		case NETWORK_PACKET_TYPE_PLAYER_CONNECT:    network_receive_player_connect(event);    break;
		case NETWORK_PACKET_TYPE_PLAYER_DISCONNECT: network_receive_player_disconnect(event); break;
		case NETWORK_PACKET_TYPE_PLAYER_POSE:       network_receive_player_pose(event);       break;
		default: break;
	}
}

void network_server_receive(const ENetEvent *event) {
	if (event->type != ENET_EVENT_TYPE_RECEIVE) return;
	switch ((NetworkPacketType)event->packet->data[0]) {
		case NETWORK_PACKET_TYPE_PLAYER_POSE: network_receive_player_pose(event); break;
		default: break;
	}
}

bool network_server_setup(const char *listen_address, int listen_port, ENetHost **host) {
	if (enet_initialize() != 0) {
		fprintf(stderr, "failed to initialise ENET\n");
		return false;
	}

	ENetAddress address;
	address.port = listen_port;
	address.host = ENET_HOST_ANY;
	if (listen_address != NULL)
		enet_address_set_host(&address, listen_address);

	*host = enet_host_create(&address, 32, 2, 0, 0);
	if (*host == NULL)  {
		fprintf(stderr, "failed to create host\n");
		return false;
	}

	return true;
}

void network_server_teardown(ENetHost *host) {
	ENetEvent event;

	for (int i = 0; i < (host ? host->peerCount : 0); i++)
		enet_peer_disconnect(host->peers+i, 0);

	while (enet_host_service(host, &event, 0) > 0);

	enet_host_destroy(host);
	enet_deinitialize();
}

void network_server_service(ENetHost *host, const uint8_t *map_data) {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_CONNECT:
				event.peer->data = player_create(event.peer);
				((Player*)event.peer->data)->position = map_get_player_spawn(map_data);
				network_broadcast_player_pose(host, event.peer->data, NULL);
				network_send_player_id(event.peer, ((Player*)event.peer->data)->id);
				network_send_map_data(event.peer, map_data);
				network_broadcast_player_connect(host, event.peer->data);
				for (int i = 0; i < host->peerCount; i++) {
					if (&host->peers[i] == event.peer) continue;
					if (host->peers[i].state != ENET_PEER_STATE_CONNECTED) continue;
					if (!((Player*)host->peers[i].data)->active) continue;
					network_send_player_connect(event.peer, host->peers[i].data);
				}
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				if (event.peer->data != NULL) {
					network_broadcast_player_disconnect(host, event.peer->data);
					player_set_active(((Player*)event.peer->data)->id, false);
					event.peer->data = NULL;
				}
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				network_server_receive(&event);
				enet_packet_destroy(event.packet);
				break;
			default:
				break;
		}
	}

	for (int i = 0; i < host->peerCount; i++)
		if (host->peers[i].state == ENET_PEER_STATE_CONNECTED)
			network_broadcast_player_pose(host, host->peers[i].data, &host->peers[i]);
}

bool network_client_setup(
	const char *server_address,
	int server_port,
	ENetHost **host,
	ENetPeer **server,
	uint8_t *map_data
) {
	if (enet_initialize() != 0) {
		fprintf(stderr, "failed to initialise ENET\n");
		return false;
	}

	ENetAddress address;
	address.port = server_port;
	enet_address_set_host(&address, server_address);

	*host = enet_host_create(NULL, 1, 2, 0, 0);
	if (*host == NULL)  {
		fprintf(stderr, "failed to create host\n");
		return false;
	}

	ENetEvent event;
	*server = enet_host_connect(*host, &address, 2, 0);
	if (*server == NULL || enet_host_service(*host, &event, 1000) <= 0)  {
		fprintf(stderr, "failed to connect to server\n");
		return false;
	}

	uint8_t loading = 3;
	while (loading) {
		enet_host_service(*host, &event, 50);
		if (event.type == ENET_EVENT_TYPE_DISCONNECT) return false;
		if (event.type != ENET_EVENT_TYPE_RECEIVE) continue;
		switch (event.packet->data[0]) {
			case NETWORK_PACKET_TYPE_PLAYER_ID:
				if (!(loading & 1)) break;
				loading ^= 1;
				network_receive_player_id(&event);
				break;
			case NETWORK_PACKET_TYPE_MAP_DATA:
				if (!(loading & 2)) break;
				loading ^= 2;
				network_receive_map_data(&event, map_data);
				break;
			default:
				network_client_receive(&event);
				break;
		}
		enet_packet_destroy(event.packet);
	}

	return true;
}

void network_client_teardown(ENetHost *host) {
	ENetEvent event;

	for (int i = 0; i < (host ? host->peerCount : 0); i++)
		enet_peer_disconnect(host->peers+i, 0);

	while (enet_host_service(host, &event, 0) > 0);

	enet_host_destroy(host);
	enet_deinitialize();
}

void network_client_service(ENetHost *host, ENetPeer *server) {
	ENetEvent event;
	while (enet_host_service(host, &event, 0) > 0) {
		switch (event.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				exit(EXIT_FAILURE);
			case ENET_EVENT_TYPE_RECEIVE:
				network_client_receive(&event);
				enet_packet_destroy(event.packet);
				break;
			default:
				break;
		}
	}

	network_send_player_pose(server, player_get_ptr());
}
