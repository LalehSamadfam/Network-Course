/*
 * udp.cpp
 *
 *  Created on: May 24, 2015
 *      Author: mbm
 */

#include "udp.h"

Frame create_udp_frame (uint32_t len) {
	/* DOES NOT SET HEADERS */

	Frame frame = create_frame (len + UDP_SIZE);

	return frame;
}

struct sr_udp * get_udp_header (Frame frame) {
	return (struct sr_udp *)(frame.data + IP_SIZE);
}

void set_udp_header (Frame frame, port_t src, port_t dst) {
	struct sr_udp * header = get_udp_header(frame);

	header -> port_src = htons (src);
	header -> port_dst = htons (dst);
	header -> udp_sum = 0;
	header -> length = htons (frame.length - IP_SIZE);
}

uint16_t get_udp_length (Frame frame) {
	struct sr_udp * header = get_udp_header(frame);

	return ntohs (header -> length);
}

port_t get_src_port (Frame frame) {
	struct sr_udp * header = get_udp_header(frame);

	return ntohs (header -> port_src);
}

port_t get_dst_port (Frame frame) {
	struct sr_udp * header = get_udp_header(frame);

	return ntohs (header -> port_dst);
}


Frame create_udp_broadcast (mac_t mac_src, ip_t ip_src, port_t port_src, port_t port_dst) {
	Frame frame = create_udp_frame(0);

	set_udp_header (frame, port_src, port_dst);
	set_ip_header(frame, ip_src, BROADCAST_IP, IPPROTO_UDP);
	set_ether_header(frame, mac_src, BROADCAST_MAC);

	return frame;
}

byte * get_udp_payload (Frame frame) {
	return frame.data + UDP_SIZE;
}

void copy_udp_payload (Frame frame, byte * data, uint32_t len) {
	byte * payload = get_udp_payload(frame);

	for (uint32_t i = 0;i < len; i ++) {
		payload [i] = data [i];
	}
}

