#include "ip.h"
#include <stdlib.h>


uint16_t checksum(byte *data, int length){
    uint16_t word;
	uint32_t sum = 0;
    for (uint16_t i = 0; i < length; i = i+2){
        word =(((uint16_t(data[i])) << 8) & 0xFF00) + (data[i + 1] & 0xFF);
        sum = sum + (uint32_t) word;
    }
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    sum = ~sum;

    return ((uint16_t) sum);
}

struct ip * get_ip_header(Frame frame) {
	struct ip * ip = (struct ip *) (frame.data + sizeof(struct sr_ethernet_hdr));

	return ip;
}

void set_default_ip_header (Frame frame) {
	struct ip * ip = get_ip_header(frame);

	srand(time(NULL));
	ip->ip_v = 4;
	ip->ip_hl = 5;
	ip->ip_ttl = 64;
	ip->ip_tos = 0;
	ip->ip_id = 0;//rand() % 100;
	ip->ip_off = 0;
	ip->ip_len = ntohs(frame.length - ETHER_SIZE);
}

void set_ip_header (Frame frame, ip_t src, ip_t dst, byte protocol) {
	struct ip * ip = get_ip_header(frame);

	set_default_ip_header(frame);
	ip -> ip_p = protocol;
	ip -> ip_src.s_addr = htonl (src);
	ip -> ip_dst.s_addr = htonl (dst);
	ip -> ip_sum = 0;
	ip -> ip_sum = htons (checksum((byte *)get_ip_header(frame), sizeof (struct ip)));
}

ip_t get_src_ip (Frame frame) {
	struct ip * ip = get_ip_header(frame);

	return ntohl (ip -> ip_src.s_addr);
}

ip_t get_dst_ip (Frame frame) {
	struct ip * ip = get_ip_header(frame);

	return ntohl (ip -> ip_dst.s_addr);
}

byte get_ip_protocol (Frame frame) {
	struct ip * ip = get_ip_header(frame);

	return ip -> ip_p;
}


bool is_mac_broadcast (Frame frame) { // TODO move this method
	return get_dst_mac (frame) == BROADCAST_MAC;
}

bool is_ip_packet (Frame frame) {
	struct sr_ethernet_hdr * ether = get_ether_header(frame);

	return ntohs (ether -> ether_type) == ETHERTYPE_IP; // TODO refactor needed! :D
}

bool is_ip_broadcast (Frame frame) {
	if (is_ip_packet(frame) == false) {
		return false;
	}

	return is_mac_broadcast(frame) && (get_dst_ip(frame) == BROADCAST_IP);
}

bool is_udp_packet (Frame frame) {
	return is_ip_packet(frame) && (get_ip_protocol (frame) == IPPROTO_UDP);
}

bool matches_network (ip_t ip, ip_t gateway,ip_t mask) {
	return ((ip & mask) == (gateway & mask));
}
