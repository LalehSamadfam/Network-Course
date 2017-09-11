//                   In the name of GOD
/**
 * Partov is a simulation engine, supporting emulation as well,
 * making it possible to create virtual networks.
 *  
 * Copyright © 2009-2014 Behnam Momeni.
 * 
 * This file is part of the Partov.
 * 
 * Partov is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Partov is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Partov.  If not, see <http://www.gnu.org/licenses/>.
 *  
 */

#include "sm.h"

#include "sr_protocol.h"
#include "interface.h"
#include "frame.h"
#include "arp.h"
#include "utils.h"
#include <netinet/in.h>
#include "sntp.h"

using namespace std;

SimulatedMachine::SimulatedMachine(const ClientFramework *cf, int count) :
		Machine(cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
}

SimulatedMachine::~SimulatedMachine() {
	// destructor...
}

void SimulatedMachine::initialize() {
	// TODO: Initialize your program here; interfaces are valid now.
	set_arp_machine(this);
}

/**
 * This method is called from the main thread.
 * Also ownership of the data of the frame is not with you.
 * If you need it, make a copy for yourself.
 *
 * You can also send frames using:
 * <code>
 *     bool synchronized sendFrame (Frame frame, int ifaceIndex) const;
 * </code>
 * which accepts one frame and the interface index (counting from 0) and
 * sends the frame on that interface.
 * The Frame class used here, encapsulates any kind of network frame.
 * <code>
 *     class Frame {
 *     public:
 *       uint32 length;
 *       byte *data;
 *
 *       Frame (uint32 _length, byte *_data);
 *       virtual ~Frame ();
 *     };
 * </code>
 */
void SimulatedMachine::processFrame(Frame frame, int ifaceIndex) {
	// TODO: process the raw frame; frame.data points to the frame's byte stream
	LOG("packet received");
	cout<< "handle kon";
	struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr *) frame.data;

	if (ntohs(eth->ether_type) == ETHERTYPE_IP
			&& frame.length
					>= sizeof(struct sr_ethernet_hdr) + sizeof(struct ip)) {
		handle_ip_packet(frame);
	} else if (ntohs(eth->ether_type) == ETHERTYPE_ARP
			&& frame.length
					>= sizeof(struct sr_ethernet_hdr) + sizeof(struct sr_arp)) {
		handle_arp_packet(frame);

	}

}

/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run() {

	string cmd;

	string ip_str;

	cin >> cmd;
	while (true) {
		if (cmd == "update") {
			cin >> ip_str;
			if (is_valid_ip(ip_str)) {
				ip_t ip = string_to_ip(ip_str);

				uint32 length = sizeof(struct sr_ethernet_hdr)
						+ sizeof(struct ip) + sizeof(struct sr_udp)
						+ sizeof(struct sr_sntp);
				byte * data = new byte[length];

				Frame frame(length, data);
				set_sntp_flag(ip, 1);
				send_sntp(frame, this, ip);

			}
		} else if (cmd == "arp") {
			cin >> ip_str;
			if (is_valid_ip(ip_str)) {
				ip_t ip = string_to_ip(ip_str);
				if (has_mac(ip))
					print_assign_mac(ip, get_mac(ip));
				else if ((has_mac(ip) == false)) {
					uint32 length = sizeof(struct sr_ethernet_hdr)
							+ sizeof(struct ip) + sizeof(struct sr_udp)
							+ sizeof(struct sr_sntp);
					byte * data = new byte[length];

					Frame frame(length, data);
					set_sntp_flag(ip, 0);
					send_arp_request(ip);

				}
			}
		}

		cin >> cmd;

	}
}

mac_t SimulatedMachine::getMac() {
	return array_to_mac(iface[0].mac);
}

ip_t SimulatedMachine::getIp() {
	return iface[0].getIp();
}

/**
 * You could ignore this method if you are not interested on custom arguments.
 */
void SimulatedMachine::parseArguments(int argc, char *argv[]) {
	// TODO: parse arguments which are passed via --args
}

