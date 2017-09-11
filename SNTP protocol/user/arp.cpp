#include "arp.h"
#include "sntp.h"

#include <map>

#include <vector>

using namespace std;

SimulatedMachine * arp_machine;

void send_arp_request (ip_t ip);

class IpBook {

	map <ip_t, mac_t> dict;

	map <ip_t, vector<Frame> > waiting_list;

	map <ip_t, int> sntp_flag;

	void enqueue (Frame frame, ip_t ip) {

			send_arp_request(ip);

		waiting_list [ip].push_back(frame);
	}

public:

	void assign_mac (ip_t ip, mac_t mac) {
		dict [ip] = mac;
		print_assign_mac(ip, mac);
		vector <Frame> queue = waiting_list [ip];

		for (uint32 i = 0;i < queue.size();i ++) {
			Frame frame = queue [i];

			send_frame(frame, mac, ip);
		}

		// TODO correct the following
		// delete waiting_list[ip];
	}

	void set_sntp_flag(ip_t ip, int flag){
		sntp_flag[ip] = flag;
	}

	bool has_mac (ip_t ip) {
		LOG("inside has mac");
		return (dict.count (ip) > 0);
	}

	bool is_waiting_for (ip_t ip) {
		return (waiting_list.count (ip) > 0);
	}

	mac_t get_mac (ip_t ip) {
		return dict [ip];
	}

	void send_frame (Frame frame, mac_t mac, ip_t ip) {
		struct sr_ethernet_hdr * ether = get_ether_header(frame);
		mac_to_array(ether -> ether_dhost, mac);
		mac_to_array(ether -> ether_shost, arp_machine -> getMac());
		ether->ether_type = htons(ETHERTYPE_IP);

		arp_machine -> sendFrame (frame, 0);

		// TODO consider deleting frame.data
	}

	void send_frame (Frame frame, ip_t ip) {
		if (has_mac(ip)) {
			send_frame (frame, get_mac (ip) , ip);
		} else {
			enqueue(frame, ip);
		}
	}

} ip_book;

void set_arp_machine (SimulatedMachine * machine) {
	arp_machine = machine;
}

struct sr_arp * get_arp_header (Frame frame) {
	return (struct sr_arp *) (frame.data + sizeof (struct sr_ethernet_hdr));
}

Frame create_arp_frame () {
	uint32 length = sizeof (struct sr_ethernet_hdr) + sizeof (struct sr_arp);
	byte * data = new byte [length];

	Frame frame (length, data);

	struct sr_arp * arp = get_arp_header(frame);
	arp -> htype = htons(1);
	arp -> ptype = htons(ETHERTYPE_IP);
	arp -> hlen = 6;
	arp -> plen = 4;

	return frame;
}

void send_arp_response (mac_t tmac, ip_t tip) {
	Frame response = create_arp_frame();

	mac_t smac = arp_machine -> getMac();
	ip_t sip = arp_machine -> getIp();

	struct sr_ethernet_hdr * ether = get_ether_header(response);
	mac_to_array(ether -> ether_dhost, tmac);
	mac_to_array(ether -> ether_shost, smac);
	ether -> ether_type = htons(ETHERTYPE_ARP);

	struct sr_arp * arp = get_arp_header(response);
	arp -> oper = htons(ARP_REPLY);

	mac_to_array(arp -> tha, tmac);
	arp -> tpa = tip;

	mac_to_array(arp -> sha, smac);
	arp -> spa = htonl(sip);

	arp_machine -> sendFrame(response, 0);

	// TODO consider deleting response -> data
}

void send_arp_request (ip_t tip) {

	Frame frame = create_arp_frame();

	mac_t smac = arp_machine -> getMac();
	ip_t sip = arp_machine -> getIp();

	mac_t tmac = - 1; // TODO improve this line

	struct sr_ethernet_hdr * ether = get_ether_header(frame);
	mac_to_array(ether -> ether_dhost, tmac);
	mac_to_array(ether -> ether_shost, smac);
	ether -> ether_type = htons(ETHERTYPE_ARP);

	struct sr_arp * arp = get_arp_header(frame);
	arp -> oper = htons(ARP_REQUEST);

	mac_to_array(arp -> sha, smac);
	arp -> spa = htonl(sip);

	mac_to_array(arp -> tha, tmac);
	arp -> tpa = htonl(tip);

	arp_machine -> sendFrame(frame, 0);
	print_arp_sent();
}

void handle_arp_response (Frame frame) {
	struct sr_arp * arp = get_arp_header(frame);

	mac_t mac = array_to_mac (arp -> sha);
	ip_t ip = ntohl (arp -> spa);

	ip_book.assign_mac(ip, mac);
}

void handle_arp_packet (Frame frame) {
	struct sr_arp *arp = get_arp_header(frame);

	LOG ("We have received an arp packet!" << endl);
	if(ntohs(arp->oper) == ARP_REQUEST) {
		send_arp_response(array_to_mac (arp -> sha), arp -> spa);
		LOG ("It was an Arp Request" << endl);
	} else {
		handle_arp_response(frame);
		LOG("IT was a response" << endl);
	}

}

void send_packet (Frame frame, ip_t ip) {
	ip_book.send_frame(frame, ip);
}

bool has_mac (ip_t ip){
	return ip_book.has_mac(ip);
}

mac_t get_mac(ip_t ip){
	return ip_book.get_mac(ip);
}

void set_sntp_flag(ip_t ip, int flag){
	ip_book.set_sntp_flag(ip, flag);
}
