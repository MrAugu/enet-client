#include <iostream>
#include <enet/enet.h>
#include "Packet.h"

ENetHost * client;
ENetAddress serverAddress;
ENetEvent event;
ENetPeer * peer;

using namespace std;

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
		while (enet_host_service(client, &event, 1000) > 0) {
			switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE:
					cout << "Received an event from the server." << endl;
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
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