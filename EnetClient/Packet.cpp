#pragma once
#include "Packet.h"
#include <string>
#include <enet/enet.h>

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

struct packet::Packet {
private:
	int index = 0;
	int length = 0;
	byte* data = new byte[61];
public:
	Packet(int delay = 0, int net_id = -1) {
		length = 61;
		int messageType = 0x4;
		int packetType = 0x1;
		int state = 0x8;

		memset(data, 0, 61);
		memcpy(data, &messageType, 4);
		memcpy(data + 4, &packetType, 4);
		memcpy(data + 8, &net_id, 4);
		memcpy(data + 16, &state, 4);
		memcpy(data + 24, &delay, 4);
	}

	~Packet() {
		delete[] data;
	}

	void append_data (string str) {
		byte* toAppend = new byte[length + 2 + str.length() + 4];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x2;
		int str_len = str.length();
		memcpy(toAppend + length + 2, &str_len, 4);
		memcpy(toAppend + length + 6, str.data(), str_len);
		length = length + 2 + str.length() + 4;
		index++;
		data[60] = (byte)index;
	}

	void append_data(int num) {
		byte* toAppend = new byte[length + 2 + 4];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x9;
		memcpy(toAppend + length + 2, &num, 4);
		length = length + 2 + 4;
		index++;
		data[60] = (byte)index;
	}

	void append_data(unsigned int uint) {
		byte* toAppend = new byte[length + 2 + 4];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x5;
		memcpy(toAppend + length + 2, &uint, 4);
		length = length + 2 + 4;
		index++;
		data[60] = (byte)index;
	}

	void append_data(float a) {
		byte* toAppend = new byte[length + 2 + 4];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x1;
		memcpy(toAppend + length + 2, &a, 4);
		length = length + 2 + 4;
		index++;
		data[60] = (byte)index;
	}

	void append_data(float a, float b) {
		byte* toAppend = new byte[length + 2 + 8];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x3;
		memcpy(toAppend + length + 2, &a, 4);
		memcpy(toAppend + length + 6, &b, 4);
		length = length + 2 + 8;
		index++;
		data[60] = (byte)index;
	}

	void append_data(float a, float b, float c) {
		byte* toAppend = new byte[length + 2 + 12];
		memcpy(toAppend, data, length);
		delete[] data;
		data = toAppend;
		toAppend[length] = index;
		toAppend[length + 1] = 0x4;
		memcpy(toAppend + length + 2, &a, 4);
		memcpy(toAppend + length + 6, &b, 4);
		memcpy(toAppend + length + 10, &c, 4);
		length = length + 2 + 12;
		index++;
		data[60] = (byte)index;
	}

	void emit(ENetPeer* peer) {
		ENetPacket* packet = enet_packet_create(data, length, 1);
		enet_peer_send(peer, 0, packet);
	}
};

void packet::send(int type, string content, ENetPeer* peer) {
	ENetPacket* packet = enet_packet_create(0, content.length() + 5, 1);
	memcpy(packet->data, &content, 4);
	memcpy((packet->data) + 4, content.c_str(), content.length());
	enet_peer_send(peer, 0, packet);
}