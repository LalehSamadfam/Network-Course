#ifndef UTILS_H
#define UTILS_H

#include <cstring>

#include "sr_protocol.h"

#include "../base/frame.h"

using namespace std;

#define FRAME_HEADER_SIZE 18

//#define LOG(x) cerr << x;

// in order to disable log
#define LOG(x) ;

typedef uint32 ip_t;

typedef long long mac_t;

mac_t array_to_mac (byte * data);

void mac_to_array (byte * data, mac_t mac);

ip_t string_to_ip (string str);

bool is_valid_ip(string str); //TODO implement this

bool is_valid_mac(string str);//TODO implement this

struct sr_ethernet_hdr * get_ether_header (Frame frame);

string ip_to_string (ip_t ip);

string mac_to_string(mac_t mac);

void print_arp_sent();

void print_assign_mac(ip_t ip, mac_t mac);

void print_time(uint32_t time);

#endif
