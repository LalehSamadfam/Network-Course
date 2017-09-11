#ifndef IP_H
#define IP_H

#include "../base/frame.h"

#include "utils.h"
#include <time.h>
#include <stdlib.h>
#include "sm.h"

struct ip * get_ip_header (Frame frame);
uint16_t checksum(byte *data, int length);

#endif
