#include <iostream>
#include <enet/enet.h>
#include "Utils.h"
#include "Creds.h"
#include "Packet.h"
#include <stdexcept>
#include <random>
#include <fstream>

ENetHost * client;
ENetAddress serverAddress;
ENetEvent event;
ENetPeer * peer;
string ServerIp = "213.179.209.168";
int luser = 0;
int ltoken = 0;
bool ctss = false;

using namespace std;
using namespace packet;

void createConnection(string ip, int port) {
	client = enet_host_create(NULL, 1, 2, 0, 0);

	if (client == NULL) {
		cout << "Unabe to create a client." << endl;
		return;
	}

	client->checksum = enet_crc32;
	enet_host_compress_with_range_coder(client);

	enet_address_set_host(&serverAddress, ip.c_str());
	serverAddress.port = port;

	cout << "Connecting to the server..." << endl;

	peer = enet_host_connect(client, &serverAddress, 2, 0);

	if (peer == NULL) {
		cout << "No available peers for initiating a connection." << endl;
		return;
	}
}

struct OnSendToServerStruct
{
	string address;
	int port;
	int userId;
	int token;
};

void OnSendToServer(string address, int port, int login_user, int login_token) {
	if (ctss != false) return;
	cout << "Switching sub-server, connecting to: address " + address + ", port " + to_string(port) + ", session user " + to_string(login_user) + ", session token " + to_string(login_token) + "." << endl;

	luser = login_user;
	ltoken = login_token;

	createConnection(address, port);
}

struct OnConsoleMessageStruct
{
	string message;
};

struct OnPlayPositionedStruct
{
	string sound;
};

struct OnSetFreezeStateStruct
{
	int state;
};

struct OnRemoveStruct
{
	string data;
};

struct OnSpawnStruct
{
	string data;
};

struct OnActionStruct
{
	string data;
};

struct SetHasGrowIDStruct
{
	int state;
	string name;
	string password;
};

struct SetHasAccountSecuredStruct
{
	int state;
};

struct OnTalkBubbleStruct
{
	int netID;
	string bubbleText;
	int type;
};

struct SetRespawnPosStruct
{
	int respawnPos;
};

struct OnEmoticonDataChangedStruct
{
	int val1;
	string emoticons;
};

struct OnSetPosStruct
{
	float x;
	float y;
};

struct OnAddNotificationStruct
{
	string image;
	string message;
	string audio;
	int val1;
};

void SerializeFromMem(byte *pSrc, int bufferSize, int *pBytesReadOut, int netId) {
	string action = "";
	byte* dataStruct = NULL;
	byte* startPtr = pSrc;
	byte *pStartPos = pSrc;
	byte count = pSrc[0]; pSrc++;
	bool isNetIdHandled = false;

	for (int i = 0; i < count; i++) {
		byte index = pSrc[0]; pSrc++;
		byte type = pSrc[0]; pSrc++;
		switch (type) {
		case 2:
		{
			int strLen;
			memcpy(&strLen, pSrc, 4); pSrc += 4;

			string v;
			v.resize(strLen);
			memcpy(&v[0], pSrc, strLen); pSrc += strLen;
			if (index == 0) {
				action = v;
				if (action == "OnSendToServer") {
					dataStruct = (byte*)new OnSendToServerStruct;
				} else if (action == "OnConsoleMessage") {
					dataStruct = (byte*)new OnConsoleMessageStruct;
				} else if (action == "OnPlayPositioned") {
					dataStruct = (byte*)new OnPlayPositionedStruct;
				} else if (action == "OnSetFreezeState") {
					dataStruct = (byte*)new OnSetFreezeStateStruct;
				} else if (action == "OnRemove") {
					dataStruct = (byte*)new OnRemoveStruct;
				} else if (action == "OnSpawn") {
					dataStruct = (byte*)new OnSpawnStruct;
				} else if (action == "OnAction") {
					dataStruct = (byte*)new OnActionStruct;
				} else if (action == "SetHasGrowID") {
					dataStruct = (byte*)new SetHasGrowIDStruct;
				} else if (action == "SetHasAccountSecured") {
					dataStruct = (byte*)new SetHasAccountSecuredStruct;
				} else if (action == "OnTalkBubble") {
					dataStruct = (byte*)new OnTalkBubbleStruct;
				} else if (action == "SetRespawnPos") {
					dataStruct = (byte*)new SetRespawnPosStruct;
				} else if (action == "OnEmoticonDataChanged") {
					dataStruct = (byte*)new OnEmoticonDataChangedStruct;
				} else if (action == "OnSetPos") {
					dataStruct = (byte*)new OnSetPosStruct;
				} else if (action == "OnAddNotification") {
					dataStruct = (byte*)new OnAddNotificationStruct;
				}
			}
			if (action == "OnSendToServer" && index == 4) {
				v.resize(v.length() - 1);
				((OnSendToServerStruct*)dataStruct)->address = v;
			} else if (action == "OnConsoleMessage" && index == 1) {
				((OnConsoleMessageStruct*)dataStruct)->message = v;
			} else if (action == "OnPlayPositioned" && index == 1) {
				((OnPlayPositionedStruct*)dataStruct)->sound = v;
			} else if (action == "OnRemove" && index == 1) {
				((OnRemoveStruct*)dataStruct)->data = v;
			} else if (action == "OnSpawn" && index == 1) {
				((OnRemoveStruct*)dataStruct)->data = v;
			} else if (action == "OnAction" && index == 1) {
				((OnRemoveStruct*)dataStruct)->data = v;
			} else if (action == "SetHasGrowID" && index == 2) {
				((SetHasGrowIDStruct*)dataStruct)->name = v;
			} else if (action == "SetHasAccountSecured" && index == 3) {
				((SetHasGrowIDStruct*)dataStruct)->password = v;
			} else if (action == "OnTalkBubble" && index == 2) {
				((OnTalkBubbleStruct*)dataStruct)->bubbleText = v;
			} else if (action == "OnEmoticonDataChanged" && index == 2) {
				((OnEmoticonDataChangedStruct*)dataStruct)->emoticons = v;
			} else if (action == "OnAddNotification" && index == 1) {
				((OnAddNotificationStruct*)dataStruct)->image = v;
			} else if (action == "OnAddNotification" && index == 2) {
				((OnAddNotificationStruct*)dataStruct)->message = v;
			} else if (action == "OnAddNotification" && index == 3) {
				((OnAddNotificationStruct*)dataStruct)->audio = v;
			} else if (index != 0) {
				cout << "String found at " + to_string((int)index) + ": " + v << endl;
			}
			break;
		}

		case 5:
		{
			int v;
			memcpy(&v, pSrc, sizeof(int));
			pSrc += sizeof(int);
			if (action == "OnSetFreezeState" && index == 1) {
				((OnSetFreezeStateStruct*)dataStruct)->state = v;
			} else if (action == "SetHasGrowID" && index == 1) {
				((SetHasGrowIDStruct*)dataStruct)->state = v;
			} else if (action == "SetHasAccountSecured" && index == 1) {
				((SetHasAccountSecuredStruct*)dataStruct)->state = v;
			} else if (action == "OnTalkBubble" && index == 1) {
				((OnTalkBubbleStruct*)dataStruct)->netID = v;
			} else if (action == "OnTalkBubble" && index == 3) {
				((OnTalkBubbleStruct*)dataStruct)->type = v;
			} else if (action == "SetRespawnPos" && index == 1) {
				((SetRespawnPosStruct*)dataStruct)->respawnPos = v;
			} else if (action == "OnAddNotification" && index == 4) {
				((OnAddNotificationStruct*)dataStruct)->val1 = v;
			} else {
				cout << "Intx found." << endl;
			}
			break;
		}
		case 9:
		{
			int v;
			memcpy(&v, pSrc, sizeof(int));
			pSrc += sizeof(int);
			if (action == "OnSendToServer" && index == 1) {
				((OnSendToServerStruct*)dataStruct)->port = v;
			} else if (action == "OnSendToServer" && index == 2) {
				((OnSendToServerStruct*)dataStruct)->token = v;
			} else if (action == "OnSendToServer" && index == 3) {
				((OnSendToServerStruct*)dataStruct)->userId = v;
			} else if (action == "OnEmoticonDataChanged" && index == 1) {
				((OnEmoticonDataChangedStruct*)dataStruct)->val1 = v;
			} else {
				//cout << "Integer found." << endl;
			}
			break;
		}

		case 1:
		{
			float v;
			memcpy(&v, pSrc, sizeof(float));
			pSrc += sizeof(float);
			cout << "Float found at " + to_string((int)index) + ": " + to_string(v) << endl;
			break;
		}

		case 3:
		{
			float v1;
			memcpy(&v1, pSrc, sizeof(float));
			pSrc += sizeof(float);
			float v2;
			memcpy(&v2, pSrc, sizeof(float));
			pSrc += sizeof(float);

			if (action == "OnSetPos" && index == 1) {
				((OnSetPosStruct*)dataStruct)->x = v1;
				((OnSetPosStruct*)dataStruct)->y = v2;
			} else {
				cout << "Vector found at " + to_string((int)index) + ": " + to_string(v1) + "x" + to_string(v2) << endl;
			}
			break;
		}

		case 4:
		{
			float v1;
			memcpy(&v1, pSrc, sizeof(float));
			pSrc += sizeof(float);
			float v2;
			memcpy(&v2, pSrc, sizeof(float));
			pSrc += sizeof(float);
			float v3;
			memcpy(&v3, pSrc, sizeof(float));
			pSrc += sizeof(float);
			cout << "Vector3 found at " + to_string((int)index) + ": " + to_string(v1) + "x" + to_string(v2) + "x" + to_string(v3) << endl;
			break;
		}

		case 8:
		{
			cout << "Uh3... Oh..." << endl;
			return;
			break;
		}

		default:
			cout << "Unknownt var type..." << endl;
			return;
		}
	}

	cout << "Tank Action: " + action << endl;

	if (action == "OnSendToServer") {
		OnSendToServer(((OnSendToServerStruct*)dataStruct)->address, ((OnSendToServerStruct*)dataStruct)->port, ((OnSendToServerStruct*)dataStruct)->userId, ((OnSendToServerStruct*)dataStruct)->token);
	} else if (action == "OnConsoleMessage") {
		fstream file;
		file.open("console.txt", ios::out | ios::app);
		file << ((OnConsoleMessageStruct*)dataStruct)->message << endl;
		file.close();
		//OnConsoleMessage(((OnConsoleMessageStruct*)dataStruct)->message);
	} else if (action == "OnPlayPositioned") {
		//OnPlayPositioned(((OnPlayPositionedStruct*)dataStruct)->sound);
	} else if (action == "OnSetFreezeState") {
		//OnSetFreezeState(((OnSetFreezeStateStruct*)dataStruct)->state);
	} else if (action == "OnRemove") {
		//OnRemove(((OnRemoveStruct*)dataStruct)->data);
	} else if (action == "OnSpawn") {
		//OnSpawn(((OnSpawnStruct*)dataStruct)->data);
	} else if (action == "OnAction") {
		//OnAction(((OnActionStruct*)dataStruct)->data);
	} else if (action == "SetHasGrowID") {
		//SetHasGrowID(((SetHasGrowIDStruct*)dataStruct)->state, ((SetHasGrowIDStruct*)dataStruct)->name, ((SetHasGrowIDStruct*)dataStruct)->password);
	} else if (action == "SetHasAccountSecured") {
		//SetHasAccountSecured(((SetHasAccountSecuredStruct*)dataStruct)->state);
	} else if (action == "OnTalkBubble") {
		//OnTalkBubble(((OnTalkBubbleStruct*)dataStruct)->netID, ((OnTalkBubbleStruct*)dataStruct)->bubbleText, ((OnTalkBubbleStruct*)dataStruct)->type);
	} else if (action == "SetRespawnPos") {
		//SetRespawnPos(((SetRespawnPosStruct*)dataStruct)->respawnPos);
	} else if (action == "OnEmoticonDataChanged") {
		//OnEmoticonDataChanged(((OnEmoticonDataChangedStruct*)dataStruct)->val1, ((OnEmoticonDataChangedStruct*)dataStruct)->emoticons);
	} else if (action == "OnSetPos") {
		//OnSetPos(((OnSetPosStruct*)dataStruct)->x, ((OnSetPosStruct*)dataStruct)->y);
	} else if (action == "OnAddNotification") {
		//OnAddNotification(((OnAddNotificationStruct*)dataStruct)->image, ((OnAddNotificationStruct*)dataStruct)->message, ((OnAddNotificationStruct*)dataStruct)->audio, ((OnAddNotificationStruct*)dataStruct)->val1);
	}

	if (dataStruct != NULL) {
		delete dataStruct;
		dataStruct = nullptr;
	}

	if (!isNetIdHandled && netId != -1) {
		cout << "Unhandled NetID " + to_string(netId) + " for event " + action << endl;
	}
}

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

string fHash = to_string((unsigned int)rand());
string sHash = to_string((unsigned int)rand());
string rid = string_to_hex(random_string(16));
string wk = string_to_hex(random_string(16));

string generateTankPacket () {
	string token = "";

	if (luser != 0 && ltoken != 0) {
	  token = "\nuser|" + to_string(luser) + "\ntoken|" + to_string(ltoken);
	  ctss = true;
	}


	//string packetText = "tankIDName|" + creds::gtUsername + "\ntankIDPass|" + creds::gtPassword + "\nrequestedName|SmileZero\nf|1\nprotocol|94\ngame_version|3.47\nlmode|0\ncbits|0\nplayer_age|100\nGDPR|1\nhash2|" + sHash + "\nmeta|4a3dd2b55315ac3eb4.com\nfhash|-716928004\nrid|" + rid + "\nplatformID|4\ndeviceVersion|0\ncountry|us\nhash|" + fHash + "\nmac|" + utils::generate_mac() + token + "\nwk|" + wk + "\nzf|13837395";
	string packetText = "tankIDName|" + creds::gtUsername + "\ntankIDPass|" + creds::gtPassword + "\nrequestedName|SmileZero\nf|1\nprotocol|112\ngame_version|3.47\nfz|7010984\nlmode|1\ncbits|0\nplayer_age|28\nGDPR|1\nhash2|-82716716\nmeta|growbrew.com\nfhash|-716928004\nrid|014D372930A17D6E0A0C32E9071C1703\nplatformID|0\ndeviceVersion|0\ncountry|us\nhash|-1663976460\nmac|2c:6f:c9:50:d0:69" + token + "\nwk|5BDD27FA30DA784971BA854321673699\nzf|1486951257";
	return packetText;
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

	createConnection(ServerIp, 17201);

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
					string logonPacket = generateTankPacket();
					//cout << logonPacket << endl;
					sendPacket(2, logonPacket, peer);
					cout << "Sent the login packet." << endl;
				} else if (messageType == 6) {
					//cout << "Authentification has failed" << endl;
					string text = get_text_pointer(event.packet);
					cout << text << endl;

					if (text.find("Authenticated|1")) {
						sendPacket(2, "action|enter_game\n", peer);
					}
				} else if (messageType == 3) {
					//cout << get_text_pointer(event.packet) << endl;
				} else if (messageType == 4) {
					byte* pointer = get_struct_pointer_from_tank(event.packet);
					cout << "Processing tank packet with id of: " << +((char)*pointer) << " Where first byte is " << to_string(pointer[0]) << endl;

					SerializeFromMem(get_extended_data_pointer(pointer), *(int*)(pointer + 52), 0, *(int*)(pointer + 4));
				} else if (messageType == 7) {
					//sendPacket(2, "action|enter_game\n", peer);
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