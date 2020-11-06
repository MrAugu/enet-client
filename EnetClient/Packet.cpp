#pragma once
#include <string>
#include <enet/enet.h>
#include "Packet.h"

using namespace std;

int packet::get_id(char* data) {
	return *data;
}

char* packet::get_data(char* data) {
	return data + 4;
}

char* packet::get_text_pointer(ENetPacket* packet) {
	char zero = 0;
	memcpy(packet->data + packet->dataLength - 1, &zero, 1);
	return (char*)(packet->data + 4);
}

int packet::get_message_type(ENetPacket* packet) {
	if (packet->dataLength > 3u) {
		return *(packet->data);
	} else {
		return -30;
	}
}

void packet::send(int type, string content, ENetPeer* peer) {
	ENetPacket* packet = enet_packet_create(0, content.length() + 5, 1);
	memcpy(packet->data, &content, 4);
	memcpy((packet->data) + 4, content.c_str(), content.length());
	enet_peer_send(peer, 0, packet);
}