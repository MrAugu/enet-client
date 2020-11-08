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

void packet::sendPacket(int type, string content, ENetPeer* peer) {
	ENetPacket* packet = enet_packet_create(0, content.length() + 5, 1);
	memcpy(packet->data, &type, 4);
	memcpy((packet->data) + 4, content.c_str(), content.length());
	enet_peer_send(peer, 0, packet);
}

byte* packet::get_struct_pointer_from_tank(ENetPacket *packet) {
	unsigned int packetLenght = packet->dataLength;
	byte* result = NULL;

	if (packetLenght >= 0x3C) {
		byte* packetData = packet->data;
		result = packetData + 4;
		if (*(BYTE*)(packetData + 16) & 8) {
			if (packetLenght < *(int*)(packetData + 56) + 60) {
				result = 0;
			}
		} else {
			int zero = 0;
			memcpy(packetData + 56, &zero, 4);
		}
	}
	return result;
}

byte* packet::get_extended_data_pointer(byte* b) {
	return (byte*)((int)(b + 56));
}