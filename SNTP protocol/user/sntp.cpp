#include "sntp.h"
#include "sr_protocol.h"
#include "sm.h"
#include <time.h>
#include "arp.h"

#define BASE_TIME  2208988800


uint16_t skew = 0;

struct sr_sntp * set_sntp_header(Frame frame){
	struct sr_sntp * sntp = (struct sr_sntp *)(frame.data + sizeof (struct sr_ethernet_hdr) + sizeof (struct ip) + sizeof (struct sr_udp));
	sntp->li = 0;
	sntp->vn = 4;
	sntp->mode = 3;
	sntp->stratum = 0;
	sntp->poll = 0;
	sntp->precision = 0;
	sntp->delay = 0;
	sntp->dispersion = 0;

return sntp;

}

struct sr_sntp * get_sntp_header(Frame frame){
	 return  (struct sr_sntp *)(frame.data + sizeof (struct sr_ethernet_hdr) + sizeof (struct ip) + sizeof (struct sr_udp));
}

Frame create_sntp_frame(Frame frame, SimulatedMachine * machine, ip_t dip) {
	struct ip * ip = get_ip_header(frame);
	ip->ip_src.s_addr = htonl(machine->getIp());
	ip->ip_dst.s_addr = htonl(dip);
	ip->ip_sum = htons(checksum((byte *)ip, sizeof(struct ip)));
	set_udp_header(frame);
	struct sr_sntp * sntp = set_sntp_header(frame);
	sntp->ref_sec = 0;
	sntp->ref_usec= 0;
	sntp->origin_sec = htonl(time(0) + BASE_TIME + skew);
	sntp->origin_usec = 0;
	sntp->trans_sec = htonl(time(0) + BASE_TIME + skew);
	sntp->trans_usec = 0;
	sntp->rec_sec = 0;
	sntp->rec_usec = 0;

	return frame;
}

void send_sntp(Frame frame , SimulatedMachine * machine, ip_t ip) {
	create_sntp_frame(frame, machine, ip);

	if (has_mac (ip)) {
		cout << "Mac of the requested server is " << mac_to_string (get_mac (ip)) << endl;
	}
	send_packet (frame, ip);
}

void handle_ip_packet(Frame frame) {
	sr_sntp * sntp = get_sntp_header(frame);
	uint32_t t;
	uint32_t t1 = ntohl(sntp->origin_sec);
	uint32_t t2 = ntohl(sntp->rec_sec);
	uint32_t t3 = ntohl(sntp->trans_sec);
	uint32_t t4 = time(0) + BASE_TIME + skew;
	uint32_t delta = (t4 - t1) - (t3 - t2);
	t = t3 + (delta/2);
	skew = t - (time(0) + BASE_TIME);
	print_time(t);

}

