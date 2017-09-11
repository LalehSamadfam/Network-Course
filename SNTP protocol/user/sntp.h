#ifndef SNTP_H
#define SNTP_H

#include "../base/frame.h"

#include "utils.h"
#include <time.h>
#include <stdlib.h>
#include "sm.h"
#include "ip.h"
#include "udp.h"


void send_sntp(Frame frame, SimulatedMachine * machine, ip_t ip);
void handle_ip_packet(Frame frame);




#endif


