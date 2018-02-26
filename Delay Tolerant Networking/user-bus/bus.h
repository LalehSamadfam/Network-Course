#ifndef UDP_H
#define UDP_H


#include "frame.h"
#include <time.h>
#include <stdlib.h>
#include <vector>

#include "sr_protocol.h"
#include "sm.h"
#include "ip.h"
#include "udp.h"
#include "utils-bus.h"

using namespace std;

class SimulatedMachine;

class CityInfo;

class Bus {
	vector <CityInfo *> cities;

	MessageBank * bank;

public:
	Bus ();

	void clear_cities_infos ();

	void add_city_info (mac_t mac, ip_t ip, ip_t mask);

	MessageBank * get_bank () const;

	void send_to_cities (Frame frame, SimulatedMachine * machine) const;

	ip_t get_mask (ip_t city_ip) const;
};

class CityInfo {
public:
	mac_t mac;
	ip_t ip;
	ip_t mask;

	CityInfo (mac_t mac, ip_t ip, ip_t mask);
};

void send_scan_request (SimulatedMachine * machine);

void handle_scan_response (Frame frame, SimulatedMachine * machine);

void send_sync_request (SimulatedMachine * machine);

void handle_sync_response(Frame frame, SimulatedMachine * machine);

#endif
