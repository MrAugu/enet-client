#include <iostream>
#include <enet/enet.h>

ENetHost * client;
ENetAddress serverAddress;
ENetEvent event;
ENetPeer * peer;
bool isConnected;

using namespace std;

int main()
{
	if (enet_initialize() != 0) {
		cout << "Unable to initialize enet." << endl;
		return 1;
	}
	else {
		cout << "Initialized enet." << endl;
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

	peer = enet_host_connect(client, &serverAddress, 2, 0);

	if (peer == NULL) {
		cout << "Unable to connect to the server." << endl;
		return 1;
	}

	// Work In Progress
	// while (enet_host_service(client, &event, 0) > 0) {
	//	 if (event.type == ENET_EVENT_TYPE_CONNECT) {
	// 		cout << "Connection established, connect event received." << endl;
	//	 }
	//	 else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
	// 		 cout << "Connection terminated by the server." << endl;
	// 	 }
	//  }

	enet_host_destroy(client);
	return 0;
}