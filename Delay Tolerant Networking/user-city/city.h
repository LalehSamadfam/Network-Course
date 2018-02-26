#ifndef UDP_H
#define UDP_H


#include "frame.h"
#include <time.h>
#include <stdlib.h>
#include "sr_protocol.h"
#include "ip.h"
#include "udp.h"
#include "sm.h"
#include "utils.h"
#include "message.h"

class SimulatedMachine;

class City {
	MessageBank * bank;

	uint8_t default_ttl;
public:
	City ();

	MessageBank * get_bank () const;

	uint8_t get_default_ttl () const;

	void set_default_ttl (uint8_t dt);
};

void handle_udp_broadcast_request (Frame frame , SimulatedMachine * machine);

Frame create_udp_broadcast_response(mac_t src_mac, ip_t src_ip, ip_t src_mask, mac_t dst_mac, ip_t dst_ip);

void read_msg_details (SimulatedMachine * machine);

void handle_sync_request(Frame frame, SimulatedMachine * machine);

void send_sync_response(SimulatedMachine * machine, mac_t dst_mac, ip_t dst_ip);

#endif
