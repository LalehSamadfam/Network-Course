#ifndef ARP
#define ARP

#include "../base/frame.h"

#include "utils.h"

#include "sm.h"

#define ARP_PACKET_SIZE 28

void set_arp_machine (SimulatedMachine * machine);

void send_arp_request (ip_t ip);
void handle_arp_packet (Frame frame);

void send_packet (Frame frame, ip_t ip);

bool has_mac(ip_t ip);

mac_t get_mac(ip_t ip);

void set_sntp_flag(ip_t ip, int flag);

#endif
