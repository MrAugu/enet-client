#include <iostream>
#include <enet/enet.h>

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
		cout << "Initialized enet." << endl;
	}
	atexit(enet_deinitialize);

	client = enet_host_create(NULL, 1, 2, 0, 0);

	if (client == NULL) {
		cout << "Unabe to create a client." << endl;
		return 1;
	}

	enet_address_set_host(&serverAddress, "213.179.209.168");
	serverAddress.port = 17201;

	peer = enet_host_connect(client, &serverAddress, 2, 0);

	if (peer == NULL) {
		cout << "Unable to connect to the server." << endl;
		return 1;
	}

	if (enet_host_service(client, &event, 30000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
		cout << "Connection has been established with the Growtopia Server." << endl;
	} else {
		cout << "Unable to establish a connection." << endl;
	}

	enet_host_destroy(client);
	return 0;
}