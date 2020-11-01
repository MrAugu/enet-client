#include <iostream>
#include <enet/enet.h>
#include "Packet.h"
#include "Utils.h"
#include "Creds.h"

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
			if (event.type == ENET_EVENT_TYPE_RECEIVE) {
				cout << "Received an event from the server." << endl;
				
				int messageType = packet::get_message_type(event.packet);
				
				if (messageType == 1) {
					string fHash = to_string((unsigned int)rand());
					string sHash = to_string((unsigned int)rand());
					string tankLogin = "tankIDName|" + creds::gtUsername + "\ntankIDPass|" + creds::gtPassword + "\nrequestedName|DuhBa\nf|0\nprotocol|38\ngame_version|3.46\nfz|13812200\nlmode|0\ncbits|0\nhash2|" + sHash + "\nmeta | " + utils::generate_meta() + "\nfhash|-716928004\nrid|" + utils::generate_rid() + "\nplatformID|0\ndeviceVersion|0\ncountry|cz\nhash|" + fHash + "\nmac|" + utils::generate_mac() + "\nwk|" + utils::generate_rid() + "\nzf|13837395";
					cout << tankLogin << endl;
					packet::send(2, tankLogin, peer);
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