/*
 * message.h
 *
 *  Created on: May 23, 2015
 *      Author: mbm
 */

#ifndef USER_COMMON_MESSAGE_H_
#define USER_COMMON_MESSAGE_H_

#include "utils.h"
#include "udp.h"

#include <set>
#include <vector>
#include <pthread.h>

using namespace std;

class Message;

class MessageBank {

	class Envelop {
	public:
		Message * message;

		bool operator < (const Envelop other) const;
	};

	set <Envelop> envelops;

	mutable pthread_mutex_t lock;

public:
	MessageBank();

	uint32_t get_count();

	Message * add_message (Message * m);

	void add_and_mark (Message * m);

	void update_ttls ();

	void remove_dead_messages ();

	void decrement_ttls () const;

	void print_messages () const;

	int get_bank_size () const;

	int size () const ;

	Frame pack_all () const;

	Frame pack_unseen () const;

	void mark_as_unseen () const;

	void remove_messages_in_range (ip_t ip, ip_t mask);
};


#define MESSAGE_HEADER_SIZE ((uint16_t) (2 + 4 + 4 + 1 + 1))

struct sr_message {
	uint16_t length;

	ip_t src_ip;

	ip_t dst_ip;

	uint8_t ttl;

	uint8_t id;
} __attribute__ ((packed));

class Message {
public:
	uint16_t length;

	ip_t src_ip;

	ip_t dst_ip;

	uint8_t ttl;

	uint8_t id;

	byte * data;

	bool seen;

	bool operator < (const Message & other) const;

	void to_byte_array (byte * array) const;

	byte * to_byte_array () const;

	uint32_t from_byte_array (byte * array);

	uint32_t calc_size () const;
};

class MessageEncoder {

	uint32_t count;

	vector <byte> data;

public:
	MessageEncoder ();

	uint32_t get_count();

	void add_message (Message * m);

	Frame to_udp_frame () const;
};

class MessageDecoder {

	uint32_t offset;

	uint32_t count;

	byte * data;

public:
	MessageDecoder (Frame frame);

	uint32_t get_count();

	bool has_next () const;

	Message * next ();
};

#endif /* USER_COMMON_MESSAGE_H_ */
