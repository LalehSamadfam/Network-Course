#include <stdlib.h>
#include "city.h"

City::City () {
	bank = new MessageBank ();
}

MessageBank * City::get_bank () const {
	return bank;
}

uint8_t City::get_default_ttl () const {
	return default_ttl;
}

void City::set_default_ttl (uint8_t dt) {
	this -> default_ttl = dt;
}


void handle_udp_broadcast_request(Frame frame, SimulatedMachine * machine){
	mac_t src_mac = machine -> getMac();
	ip_t src_ip = machine -> getIp();
	ip_t src_mask = machine -> getMask();

	mac_t dst_mac = get_src_mac(frame);
	ip_t dst_ip = get_src_ip(frame);
	Frame response = create_udp_broadcast_response(src_mac, src_ip, src_mask, dst_mac, dst_ip);
	machine->sendFrame(response, 0);
}

Frame create_udp_broadcast_response(mac_t src_mac, ip_t src_ip, ip_t src_mask, mac_t dst_mac, ip_t dst_ip) {

	Frame response = create_udp_frame(sizeof(CityIntro));

	CityIntro intro;
	intro.ip = htonl (src_ip & src_mask);
	intro.mask = htonl (src_mask);

	copy_udp_payload(response, (byte *)&intro, sizeof(CityIntro));

	set_udp_header(response, 5000, 5000);
	set_ip_header(response, src_ip, dst_ip, IPPROTO_UDP);
	set_ether_header(response, src_mac, dst_mac);

	return response;
}

void read_msg_details (SimulatedMachine * machine) {
	City * city = machine -> getCity ();

	string src_ip;
	string dst_ip;
	int id;
	int ttl = city -> get_default_ttl ();
	string data;

	cin >> src_ip;
	cin >> dst_ip;
	cin >> id;

	getline (cin, data, ' ');
	getline (cin, data);

	PRN(data);
	PRN(data.size());
	PRN(data.size() + MESSAGE_HEADER_SIZE);
	Message * m = new Message ();
	m -> length = MESSAGE_HEADER_SIZE + data.size ();
	m -> src_ip = string_to_ip(src_ip);
	m -> dst_ip = string_to_ip(dst_ip);
	m -> ttl = ttl;
	m -> id = id;
	m -> data = new byte [m -> length - MESSAGE_HEADER_SIZE];

	for (uint32_t i = 0;i < data.size();i ++) {
		m -> data [i] = data [i];
	}
	//m -> data [m -> length - MESSAGE_HEADER_SIZE - 1] = 0;

	city -> get_bank() -> add_message(m);
}

void handle_sync_request(Frame frame, SimulatedMachine * machine) {
	// TODO review this method
	MessageDecoder decoder (frame);
	MessageBank * bank = machine -> getCity() -> get_bank();
	
	uint32_t count = decoder.get_count();
	cout << count << " messages are received from " << ip_to_string(get_src_ip(frame)) << endl;

	ip_t city_ip = machine -> getIp ();
	ip_t city_mask = machine -> getMask ();

	bank -> mark_as_unseen();
	while (decoder.has_next ()) {
		Message * m = decoder.next ();

		if (matches_network (m -> dst_ip, city_ip, city_mask)) {
			cout << "A message received which is owned by this city" << endl;
		} else {
			bank->add_and_mark(m);
		}
	}

	mac_t dst_mac = get_src_mac (frame);
	ip_t dst_ip = get_src_ip (frame);
LOG("before send sync response");
	send_sync_response(machine, dst_mac, dst_ip);

	bank -> update_ttls();
}

void send_sync_response(SimulatedMachine * machine, mac_t dst_mac, ip_t dst_ip) {
	LOG("start of send sync response");
	City * city = machine -> getCity();
	MessageBank * bank = city -> get_bank();
	uint32_t count = bank->get_count();

LOG("before pack unseen");
	Frame frame = bank -> pack_unseen();

	mac_t src_mac = machine -> getMac();
	ip_t src_ip = machine -> getIp();
	port_t src_port = 8001;

	port_t dst_port = 3000;

	set_udp_header(frame, src_port, dst_port);
	set_ip_header(frame, src_ip, dst_ip, IPPROTO_UDP);
	set_ether_header(frame, src_mac, dst_mac);

	cout << count <<" messages are sent to " <<ip_to_string(dst_ip) << endl;
	machine -> sendFrame(frame, 0);
}
