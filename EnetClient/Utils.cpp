#pragma once
#include "Utils.h"
#include <string>

using namespace std;

char utils::hexMap[] = {
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
	'A', 'B', 'C', 'D', 'E', 'F'
};

string utils::hex_string(unsigned char data) {
	string str(2, ' ');
	str[0] = utils::hexMap[(data & 0xF0) >> 4];
	str[1] = utils::hexMap[data & 0xF0];
	return str;
}

string utils::generate_meta() {
	string meta;
	for (int i = 0; i < 9; i++) {
		meta += utils::hex_string(rand());
	}
    meta += ".com";
	return meta;
}

string utils::generate_mac() {
	string mac;
	for (int i = 0; i < 6; i++) {
		mac += utils::hex_string(rand());
		if (i != 5) mac += ":";
	}
	return mac;
}

string utils::generate_rid() {
	string rid;
	for (int i = 0; i < 16; i++) {
		rid += utils::hex_string(rand());
	}

	for (auto & ch : rid) ch = toupper(ch);
	
	return rid;
}