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
#include <netinet/in.h>

#include "utils-bus.h"
#include "bus.h"

using namespace std;

SimulatedMachine::SimulatedMachine(const ClientFramework *cf, int count) :
		Machine(cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
	bus = new Bus ();
}

SimulatedMachine::~SimulatedMachine() {
	// destructor...
}

void SimulatedMachine::initialize() {
	// TODO: Initialize your program here; interfaces are valid now.
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
	// following code performs some sample checks. Replace them with your own complete checks....

	if (is_udp_packet(frame)){
		port_t port = get_dst_port (frame);

		switch (port) {
		case 5000:
			handle_scan_response(frame, this);
			break;
		case 3000:
			handle_sync_response(frame, this);
			break;
		}
	}
}

/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run() {
	// TODO: write your business logic here...
	do {
		string command;
		cin >> command;
		if (command == "print") {
			bus->get_bank()->print_messages();
		} else if (command == "walk") {
			walk();
			bus->clear_cities_infos(); // TODO remove this line if needed!
		} else if (command == "scan") {
			send_scan_request(this);
		} else if (command == "sync") {
			LOG("command sync has been entered");
			send_sync_request(this);
		}
	} while (true);

}

mac_t SimulatedMachine::getMac() {
	return array_to_mac(iface[0].mac);
}

ip_t SimulatedMachine::getIp() {
	return iface[0].getIp();
}

Bus * SimulatedMachine::getBus () const {
	return bus;
}

/**
 * You could ignore this method if you are not interested on custom arguments.
 */
void SimulatedMachine::parseArguments(int argc, char *argv[]) {
	// TODO: parse arguments which are passed via --args
}

