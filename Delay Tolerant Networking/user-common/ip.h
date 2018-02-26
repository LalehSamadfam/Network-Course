#ifndef IP_H
#define IP_H

#include "frame.h"

#include <time.h>
#include <stdlib.h>

#include "utils.h"

#include "sr_protocol.h"

#define IP_SIZE (ETHER_SIZE + sizeof (struct ip))

#define BROADCAST_IP ((ip_t) -1)

#define BROADCAST_MAC ((((mac_t) 1) << 48) -1)


struct ip * get_ip_header (Frame frame);

uint16_t checksum(byte *data, int length);

void set_ip_header (Frame frame, ip_t src, ip_t dst, byte protocol);

ip_t get_src_ip (Frame frame);

bool is_mac_broadcast (Frame frame);

bool is_ip_packet (Frame frame);

bool is_ip_broadcast (Frame frame);

bool is_udp_packet (Frame frame);

bool matches_network (ip_t ip, ip_t gateway,ip_t mask);

#endif
