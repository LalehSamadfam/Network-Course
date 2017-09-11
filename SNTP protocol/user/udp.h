#ifndef UDP_H
#define UDP_H


#include "../base/frame.h"
#include <time.h>
#include <stdlib.h>
#include "sr_protocol.h"

struct sr_udp * set_udp_header(Frame frame);

#endif
