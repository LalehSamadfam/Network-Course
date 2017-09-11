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

using namespace std;

SimulatedMachine::SimulatedMachine (const ClientFramework *cf, int count) :
	Machine (cf, count) {
	// The machine instantiated.
	// Interfaces are not valid at this point.
}

SimulatedMachine::~SimulatedMachine () {
	// destructor...
}

void SimulatedMachine::initialize () {
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
void SimulatedMachine::processFrame (Frame frame, int ifaceIndex) {
	// TODO: process the raw frame; frame.data points to the frame's byte stream
  // following code performs some sample checks. Replace them with your own complete checks....
	cerr << "Frame received at iface " << ifaceIndex <<
		" with length " << frame.length << endl;
  struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr *) frame.data;
  if (ntohs (eth->ether_type) == ETHERTYPE_IP
      && frame.length >= sizeof (struct sr_ethernet_hdr) + sizeof (struct ip)) {
    struct ip *packet = (struct ip *) (frame.data + sizeof (struct sr_ethernet_hdr));
    struct in_addr dstip = packet->ip_dst; // in network byte order
    cerr << "dst ip is " << inet_ntoa (dstip) << endl;
  }
}


/**
 * This method will be run from an independent thread. Use it if needed or simply return.
 * Returning from this method will not finish the execution of the program.
 */
void SimulatedMachine::run () {
	// TODO: write your business logic here...
  // following code sends a sample packet. Do not forget to remove it....
  const int frameLength = sizeof (struct sr_ethernet_hdr) + sizeof (struct sr_arp);
  byte *data = new byte[frameLength];

  struct sr_ethernet_hdr *eth = (struct sr_ethernet_hdr *) data;
  memset (eth->ether_dhost, 255, ETHER_ADDR_LEN); // broadcast address
  memcpy (eth->ether_shost, iface[0].mac, ETHER_ADDR_LEN);
  eth->ether_type = htons (ETHERTYPE_ARP);

  struct sr_arp *arp = (struct sr_arp *) (data + sizeof (struct sr_ethernet_hdr));
  (void) arp; // set fields of ARP header.......

  Frame frame (frameLength, data);
  sendFrame (frame, 0); // sends frame on interface 0
  delete[] data;
  cerr << "now ./free.sh and check the pcap log file to see the sent packet" << endl;
}


/**
 * You could ignore this method if you are not interested on custom arguments.
 */
void SimulatedMachine::parseArguments (int argc, char *argv[]) {
	// TODO: parse arguments which are passed via --args
}

