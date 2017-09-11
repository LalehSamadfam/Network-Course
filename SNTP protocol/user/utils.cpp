#include "utils.h"
#include <string.h>
#include <cstdlib>
#include <iostream>
#include <sstream>

using namespace std;

mac_t array_to_mac (byte * data) {
	mac_t mac = 0;

	for (int i = 0;i < ETHER_ADDR_LEN;i ++) {
		mac = (mac << 8) + data [i];
	}

	return mac;
}

void mac_to_array (byte * data, mac_t mac) {
	for (int i = ETHER_ADDR_LEN - 1;i >= 0;i --) {
		data [i] = mac & ((1 << 8) - 1);
		mac = mac >> 8;
	}
}

int string_to_int (string s) {
	int result = 0;

	for (uint32 i = 0;i < s.length();i ++) {
		result = result * 10 + (s [i] - '0');
	}

	return result;
}

ip_t string_to_ip (string str) {
	int start = 0;
	ip_t ip = 0;
	for (uint32 i = 0;i < str.length();i ++) {
		if (str [i] == '.') {
			ip = (ip << 8) + string_to_int (str.substr(start, i - start));
			start = i + 1;
		}
	}
	ip = (ip << 8) + string_to_int(str.substr(start));

	return ip;
}

bool is_valid_ip(string str){

	return true;
}

struct sr_ethernet_hdr * get_ether_header (Frame frame) {
	return (struct sr_ethernet_hdr *) (frame.data);
}

string hex_to_string(byte b) {
	string result = "";

	int bb = b & ((1 << 8) - 1);
	int right = bb % 16;
	int left = bb / 16;

	if (left < 10) {
		result += ('0' + left);
	} else {
		result += ('a' + left - 10);
	}

	if (right < 10) {
		result += ('0' + right);
	} else {
		result += ('a' + right - 10);
	}

	return result;
}
string mac_to_string(mac_t mac) {
	string result = "";

	bool is_first = true;
	for (int i = 0; i < 6; i++) {
		if (is_first == true) {
			is_first = false;
		} else {
			result = ":" + result;
		}
		result = ::hex_to_string((byte) mac) + result;
		mac = mac >> 8;
	}

	return result;
}

string ip_to_string (ip_t ip) {
	unsigned char octets [4];

	for (int i = 0;i < 4;i ++) {
		octets [i] = ip % 256;
		ip /= 256;
	}

	stringstream ss;
	string result;

	ss << int(octets[3]) << '.' << int(octets[2]) << '.' << int(octets[1]) << '.' << int(octets[0]);
	ss >> result;

	return result;

}

void print_arp_sent(){
	cout << "Arp request sent" << endl;
}

void print_assign_mac(ip_t ip, mac_t mac){
	cout << "ip " << ip_to_string(ip) << " mapped to " << mac_to_string(mac)<<endl;
}

void print_time(uint32_t time){
	cout << time << endl;
}
