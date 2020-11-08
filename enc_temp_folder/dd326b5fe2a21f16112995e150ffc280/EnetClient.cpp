#include <iostream>
#include <enet/enet.h>
#include "Utils.h"
#include "Creds.h"
#include "Packet.h"
#include <stdexcept>
#include <random>

ENetHost * client;
ENetAddress serverAddress;
ENetEvent event;
ENetPeer * peer;

using namespace std;
using namespace packet;


string string_to_hex(const string& input) {
	static const char hex_digits[] = "0123456789ABCDEF";

	string output;
	output.reserve(input.length() * 2);
	for (unsigned char c : input) {
		output.push_back(hex_digits[c >> 4]);
		output.push_back(hex_digits[c & 15]);
	}
	return output;
}

int hex_value(unsigned char hex_digit) {
	static const signed char hex_values[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	};
	int value = hex_values[hex_digit];
	if (value == -1) throw invalid_argument("invalid hex digit");
	return value;
}

string hex_to_string(const string& input) {
	const auto len = input.length();
	if (len & 1) throw invalid_argument("odd length");

	string output;
	output.reserve(len / 2);
	for (auto it = input.begin(); it != input.end(); )
	{
		int hi = hex_value(*it++);
		int lo = hex_value(*it++);
		output.push_back(hi << 4 | lo);
	}
	return output;
}

string random_string(string::size_type length) {
	static auto& chrs = "0123456789"
		"abcdefghijklmnopqrstuvwxyz"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	thread_local static mt19937 rg{ random_device{}() };
	thread_local static uniform_int_distribution<string::size_type> pick(0, sizeof(chrs) - 2);

	string s;

	s.reserve(length);

	while (length--)
		s += chrs[pick(rg)];

	return s;
}

int main()
{
	if (enet_initialize() != 0) {
		cout << "Unable to initialize enet." << endl;
		return 1;
	}
	else {
		cout << "Initializing networking library..." << endl;
	}
	atexit(enet_deinitialize);

	client = enet_host_create(NULL, 1, 2, 0, 0);

	if (client == NULL) {
		cout << "Unabe to create a client." << endl;
		return 1;
	}

	client->checksum = enet_crc32;
	enet_host_compress_with_range_coder(client);

	enet_address_set_host(&serverAddress, "213.179.209.168");
	serverAddress.port = 17201;

	cout << "Connecting to the server..." << endl;

	peer = enet_host_connect(client, &serverAddress, 2, 0);

	if (peer == NULL) {
		cout << "No available peers for initiating a connection." << endl;
		return 1;
	}

	if (enet_host_service(client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		cout << "Connected to the server." << endl;
	} else {
		enet_peer_reset(peer);
		cout << "Unable to connect to the server." << endl;
		return 1;
	}

	while (1) {
		while (enet_host_service(client, &event, 0) > 0) {
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				int messageType = get_message_type(event.packet);
				cout << "Received an event from the server, of the type " + to_string(messageType) + "." << endl;

				if (messageType == 1) {
					//string fHash = to_string((unsigned int)rand());
					//string sHash = to_string((unsigned int)rand());
					//string fz = to_string((unsigned int)rand());
					//string zf = to_string((unsigned int)rand());
					string sHash = "-82716716";
					string fHash = "-1663976460";
					string fz = "7010984";
					string zf = "1486951257";

					string rid = string_to_hex(random_string(16));
					string wk = string_to_hex(random_string(16));
					string gameVersion = "3.47";
					string country = "cz";
					string packetText = "";
					string mac = "2c:6f:c9:50:d0:68";

					if (creds::gtPassword != "" && creds::gtUsername != "") {
						packetText += "tankIDName|" + (creds::gtUsername + "\n");
						packetText += "tankIDPass|" + (creds::gtPassword + "\n");
					}
					
					packetText += "requestedName|SickleTiny\n";
					packetText += "f|1\n";
					packetText += "protocol|112\n";
					packetText += "game_version|" + (gameVersion + "\n");
					packetText += "fz|"+ (fz + "\n");
					packetText += "lmode|0\n";
					packetText += "cbits|0\n";
					packetText += "player_age|28\n";
					packetText += "GDPR|1\n";
					packetText += "hash2|" + (sHash + "\n");
					packetText += "meta|growbrew.com\n";
					packetText += "fhash|" + (sHash + "\n");
					packetText += "rid|" + (rid + "\n");
					packetText += "platformID|4\n";
					packetText += "deviceVersion|0\n";
					packetText += "country|" + (country + "\n");
					packetText += "hash|" + (fHash + "\n");
					packetText += "mac|" + (mac + "\n");
					packetText += "wk|" + (wk + "\n");
					packetText += "zf|" + (zf + "\n");
					
					cout << packetText << endl;

					sendPacket(2, packetText, peer);

					cout << "Sent the login tank packet." << endl;
				} else if (messageType == 6) {
					cout << get_text_pointer(event.packet) << endl;
					// Getting:

				}

				break;
			} else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
				cout << "Disconnected from the server." << endl;
				// Insert Disconnection Handling Code
				break;
			}
		}
	}

	enet_host_destroy(client);
	enet_deinitialize();

	return 0;
}