#include "udp.h"
#include <stdlib.h>

struct sr_udp * set_udp_header(Frame frame){
	struct sr_udp * udp = (struct sr_udp *)(frame.data + sizeof (struct sr_ethernet_hdr) + sizeof (struct ip));
	srand(time(NULL));
	udp->port_src =ntohs(123);
	udp->port_dst = ntohs(123);
	udp->udp_sum = 0;
	udp->length = ntohs(56);
	return udp;
}
