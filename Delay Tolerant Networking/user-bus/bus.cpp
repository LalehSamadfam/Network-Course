#include "bus.h"

#include <stdlib.h>

/* Bus methods */
Bus::Bus () {
	bank = new MessageBank ();
}

void Bus::clear_cities_infos () {
	cities.clear();
}

void Bus::add_city_info (mac_t mac, ip_t ip, ip_t mask) {
	cities.push_back (new CityInfo (mac, ip, mask));
}

MessageBank * Bus::get_bank () const {
	return bank;
}

void Bus::send_to_cities (Frame frame, SimulatedMachine * machine) const {
	mac_t src_mac = machine -> getMac ();
	ip_t src_ip = machine -> getIp ();
	port_t src_port = 2000;
	uint32_t count = this->get_bank()->get_count();

	port_t dst_port = 8000;
	PRN (cities.size());
	for (uint32_t i = 0;i < cities.size();i ++) {
		mac_t dst_mac = cities [i] -> mac;
		ip_t dst_ip = cities [i] -> ip;

		set_udp_header(frame, src_port, dst_port);
		set_ip_header(frame, src_ip, dst_ip, IPPROTO_UDP);
		set_ether_header(frame, src_mac, dst_mac);

		machine -> sendFrame(frame, 0);
		cout << count << " messages are sent to " << ip_to_string(dst_ip) <<endl;
	}

}

ip_t Bus::get_mask (ip_t city_ip) const {
	for (uint32_t i = 0;i < cities.size ();i ++) {
		if (cities [i] -> ip == city_ip) {
			return cities [i] -> mask;
		}
	}

	// Error!
	return 4294967095;
}

/* CityInfo methods */
CityInfo::CityInfo (mac_t mac, ip_t ip, ip_t mask):mac(mac), ip(ip), mask(mask) {

}

/* methods */
void send_scan_request (SimulatedMachine * machine) {
	mac_t my_mac = machine -> getMac();
	ip_t my_ip = machine -> getIp();
	Frame frame = create_udp_broadcast(my_mac, my_ip, 5000, 5000);

	machine -> sendFrame(frame, 0);
}

void handle_scan_response (Frame frame, SimulatedMachine * machine) {
	byte * payload = get_udp_payload(frame);

	struct CityIntro * intro = (struct CityIntro *) payload;

	mac_t mac = get_src_mac(frame);
	ip_t ip = get_src_ip(frame);
	ip_t mask = ntohl (intro -> mask);

	cout << "City with " << ip_to_string(ip & mask) << "/" << mask_to_int(mask) << " is in range" << endl;

	machine -> getBus () -> add_city_info(mac, ip, mask);
}

void send_sync_request(SimulatedMachine * machine) {
	LOG ("before sending sync db")
	Bus * bus = machine -> getBus();
	MessageBank * bank = bus -> get_bank();

	Frame frame = bank -> pack_all();

	bus -> send_to_cities (frame, machine);
}

void handle_sync_response(Frame frame, SimulatedMachine * machine) {
	Bus * bus = machine -> getBus ();

	MessageDecoder decoder (frame);
	MessageBank * bank = bus -> get_bank();

	uint32_t count = decoder.get_count();
	cout << count << " messages are received from " << ip_to_string(get_src_ip(frame)) << endl;

	while (decoder.has_next ()) {
		Message * m = decoder.next ();
		bank->add_message(m);
	}

	bank -> update_ttls();

	ip_t city_ip = get_src_ip (frame);
	ip_t city_mask = bus -> get_mask (city_ip);
	bank -> remove_messages_in_range (city_ip, city_mask);
}
