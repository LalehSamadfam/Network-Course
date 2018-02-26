#ifndef UTILS_BUS_H
#define UTILS_BUS_H

#include "utils.h"
#include "message.h"

struct sync_payload{
	int number;
	byte* messages;
}__attribute__ ((packed)) ;


#endif
