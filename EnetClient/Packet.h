#pragma once
#include <enet/enet.h>
#include <string>

typedef unsigned char byte;
using namespace std;

namespace packet {
	int get_id(char* data);
	char* get_data(char* data);
	char* get_text_pointer(ENetPacket* packet);
	int get_message_type(ENetPacket* packet);
	void send(int type, string content, ENetPeer* peer);
	struct Packet;
}