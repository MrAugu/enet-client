#pragma once
#include <string>

using namespace std;

namespace utils {
	char hexMap[];
	string hex_string(unsigned char data);
	string generate_meta();
	string generate_mac();
	string generate_rid();
}