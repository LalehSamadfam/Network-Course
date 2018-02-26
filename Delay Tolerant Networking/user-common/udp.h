/*
 * udp.h
 *
 *  Created on: May 24, 2015
 *      Author: mbm
 */

#ifndef USER_COMMON_UDP_H_
#define USER_COMMON_UDP_H_

#include "utils.h"
#include "ip.h"

#define UDP_SIZE (IP_SIZE + sizeof (struct sr_udp))

Frame create_udp_frame (uint32_t len);

struct sr_udp * get_udp_header (Frame frame);

void set_udp_header (Frame frame, port_t src, port_t dst);

uint16_t get_udp_length (Frame frame);

port_t get_src_port (Frame frame);

port_t get_dst_port (Frame frame);

Frame create_udp_broadcast (mac_t mac_src, ip_t ip_src, port_t port_src, port_t port_dst);

byte * get_udp_payload (Frame frame);

void copy_udp_payload (Frame frame, byte * data, uint32_t len);

#endif /* USER_COMMON_UDP_H_ */
