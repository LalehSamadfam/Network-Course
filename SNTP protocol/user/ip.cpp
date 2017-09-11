#include "ip.h"
#include <stdlib.h>


uint16_t checksum(byte *data, int length){
    uint16_t word;
	uint32_t sum = 0;

    for (uint16_t i = 0; i < length; i = i+2){
        word =((data[i] << 8) & 0xFF00) + (data[i + 1] & 0xFF);
        sum = sum + (uint32_t) word;
    }
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    sum = ~sum;

    return ((uint16_t) sum);
}

struct ip * get_ip_header(Frame frame) {
	struct ip * ip = (struct ip *) (frame.data + sizeof(struct sr_ethernet_hdr));
	srand(time(NULL));
	ip->ip_v = 4;
	ip->ip_hl = 5;
	ip->ip_ttl = 64;
	ip->ip_tos = 0;
	ip->ip_id = rand() % 100;
	ip->ip_off = 0;
	ip->ip_p = 17;
	ip->ip_len = ntohs(76);

	return ip;
}
