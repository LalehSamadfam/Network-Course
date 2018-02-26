/*
 * utils.h
 *
 *  Created on: May 23, 2015
 *      Author: mbm
 */

#ifndef USER_COMMON_UTILS_H_
#define USER_COMMON_UTILS_H_


#include <cstring>

#include "sr_protocol.h"

#include "../base/frame.h"

using namespace std;

#define ETHER_SIZE (sizeof(struct sr_ethernet_hdr))

#define LOG(x) ;
//#define LOG(x) cerr << x << endl;
#define PRN(x) ;
//#define PRN(x) cerr << #x << " = " << x << endl;

// in order to disable log
//#define LOG(x) ;

typedef uint16_t port_t;

typedef uint32_t ip_t;

typedef long long mac_t;

mac_t array_to_mac (byte * data);

void mac_to_array (byte * data, mac_t mac);

mac_t get_src_mac (Frame frame);

mac_t get_dst_mac (Frame frame);

int string_to_int (string s);

ip_t string_to_ip (string str);

bool is_valid_ip(string str); //TODO implement this

bool is_valid_mac(string str);//TODO implement this

struct sr_ethernet_hdr * get_ether_header (Frame frame);

void set_ether_header (Frame frame, mac_t src, mac_t dst);

string ip_to_string (ip_t ip);

string mac_to_string(mac_t mac);

Frame create_frame(int frame_length);

void print_arp_sent();

void print_assign_mac(ip_t ip, mac_t mac);

void print_time(uint32_t time);

int mask_to_int (ip_t mask);

struct CityIntro
{
	uint32_t mask;
	uint32_t ip;
} __attribute__ ((packed)) ;

#endif /* USER_COMMON_UTILS_H_ */
