#include "message.h"

MessageBank::MessageBank(){
	pthread_mutex_init (&lock, NULL);
}

uint32_t MessageBank::get_count(){
	pthread_mutex_lock(& lock);
	uint32_t size = envelops.size();
	pthread_mutex_unlock(& lock);

	return size;
}

bool MessageBank::Envelop::operator < (const Envelop other) const {
	return (* message) < (* (other.message));
}


Message * MessageBank::add_message (Message * m) {
	Envelop e;

	e.message = m;

	pthread_mutex_lock(& lock);
	if (envelops.count(e) == 0) {
		envelops.insert(e);
	} else {
		m = (envelops.find(e)) -> message;
	}
	pthread_mutex_unlock(& lock);

	return m;
}

void MessageBank::add_and_mark (Message * m) {
	m = add_message(m);
	m -> seen = true;
}

void MessageBank::update_ttls () {
	decrement_ttls ();
	remove_dead_messages();
}

void MessageBank::remove_dead_messages() {
	pthread_mutex_lock(& lock);
	for (set<Envelop>::iterator it = envelops.begin (); it != envelops.end ();) {
		if (it -> message -> ttl == 0) {
			envelops.erase (it ++);
		} else {
			++ it;
		}
	}
	pthread_mutex_unlock(& lock);
}

void MessageBank::decrement_ttls () const {
	pthread_mutex_lock(& lock);
	for (set <Envelop>::iterator it = envelops.begin(); it != envelops.end(); it ++) {
		(it -> message) -> ttl --;
	}
	pthread_mutex_unlock(& lock);
}

void MessageBank::print_messages () const {
	pthread_mutex_lock(& lock);
	if (envelops.empty()) {
		cout << "No message" << endl;
		pthread_mutex_unlock(& lock);
		return;
	}

	for (set <Envelop>::iterator it = envelops.begin(); it != envelops.end(); it ++) {
		Message * message = it -> message;
		cout << "Dst IP: " << ip_to_string(message -> dst_ip) << " | "
				<< "Src IP: " << ip_to_string(message -> src_ip) << " | "
				<< "ID: " << int(message -> id) << " | "
				<< "TTL: " << int(message -> ttl) << " | "
				<< message -> data << endl;
	}
	pthread_mutex_unlock(& lock);
}

int  MessageBank::size () const {
	pthread_mutex_lock(& lock);
	int size = envelops.size();
	pthread_mutex_unlock(& lock);

	return size;
}

Frame MessageBank::pack_all () const {
	MessageEncoder encoder;

	pthread_mutex_lock(& lock);
	for (set <Envelop>::iterator it = envelops.begin(); it != envelops.end(); it ++) {
		Message * message = it -> message;
		LOG(message);
		encoder.add_message(message);
	}
	pthread_mutex_unlock(& lock);

	return encoder.to_udp_frame();
}

Frame MessageBank::pack_unseen () const {
	MessageEncoder encoder;

	pthread_mutex_lock(& lock);
	for (set <Envelop>::iterator it = envelops.begin(); it != envelops.end(); it ++) {
		Message * message = it -> message;
		if (message -> seen == false) {
			LOG("before add message");
			encoder.add_message(message);
		}
	}
	pthread_mutex_unlock(& lock);
	LOG("before to udp frame");
	return encoder.to_udp_frame();
}

void MessageBank::mark_as_unseen () const {
	pthread_mutex_lock(& lock);
	for (set <Envelop>::iterator it = envelops.begin(); it != envelops.end(); it ++) {
		Message * message = it -> message;
		message -> seen = false;
	}
	pthread_mutex_unlock(& lock);
}

void MessageBank::remove_messages_in_range (ip_t ip, ip_t mask) {
	pthread_mutex_lock(& lock);
	for (set<Envelop>::iterator it = envelops.begin (); it != envelops.end ();) {
		if (matches_network (it -> message -> dst_ip, ip, mask)) {
			cout << "Message with dst " << ip_to_string(it->message ->dst_ip) << "and src " << ip_to_string(it->message ->src_ip) << " arrived to its destination" << endl;
			envelops.erase (it ++);
		} else {
			++ it;
		}
	}
	pthread_mutex_unlock(& lock);
}

void Message::to_byte_array (byte * array) const {
	sr_message * t = (sr_message *) array;
	t->length = htons (length);
	t->src_ip = htonl (src_ip);
	t->dst_ip = htonl (dst_ip);
	t->ttl = ttl;
	t->id = id;
	PRN(length);
	for (int i = 0;i < length - MESSAGE_HEADER_SIZE;i ++) {
		array [MESSAGE_HEADER_SIZE + i] = data [i];
	}
}

byte * Message::to_byte_array () const {
	byte * array = new byte [calc_size ()];
	LOG("before second to byte array");
	to_byte_array(array);

	return array;
}

uint32_t Message::from_byte_array (byte * array) {
	sr_message * t = (sr_message *) array;
	length = ntohs (t->length);
	src_ip = ntohl (t->src_ip);
	dst_ip = ntohl (t->dst_ip);
	ttl = t->ttl;
	id = t->id;

	data = new byte [length - MESSAGE_HEADER_SIZE + 1];

	data [length - MESSAGE_HEADER_SIZE] = 0;

	for (uint16_t i = 0;i < length - MESSAGE_HEADER_SIZE;i ++) {
		data [i] = array [MESSAGE_HEADER_SIZE + i];
	}

	return length;
}

uint32_t Message::calc_size () const {
	uint32_t size = length;

	return size;
}

bool Message::operator < (const Message & other) const {
	if (src_ip != other.src_ip) {
		return src_ip < other.src_ip;
	}
	return id < other.id;
}

/* MessageEncoder methods */
MessageEncoder::MessageEncoder ():count (0) {}

uint32_t MessageEncoder::get_count(){
	return count;
}

void MessageEncoder::add_message (Message * m) {
	count ++;
	LOG("before calc size");
	uint32_t size = m -> calc_size ();
	LOG("before to byte array");
	byte * array = m -> to_byte_array();
	LOG("before for loop");
	PRN(size);
	for (uint32_t i = 0;i < size;i ++) {
	//	cout << int(array [i]) << " ";
		data.push_back(array [i]);
	}

}

Frame MessageEncoder::to_udp_frame () const {
	// this method just sets the payload of the frame
	uint32_t total_size = 4 + data.size ();

	//PRN(data);
	PRN(data.size());
	Frame frame = create_udp_frame(total_size);
	byte * payload = get_udp_payload(frame);

	for (uint32_t i = 0;i < data.size ();i ++) {
		payload [4 + i] = data [i];
	}

	(*((uint32_t *) payload)) = htonl (count);

	return frame;
}

/* MessageDecoder methods */

MessageDecoder::MessageDecoder (Frame frame) {
	byte * payload = get_udp_payload(frame);
	uint32_t size = get_udp_length(frame) - 4;

	this -> offset = 0;
	this -> count = ntohl ((*((uint32_t *) payload)));
	this -> data = new byte [size];

	for (uint32_t i = 0;i < size;i ++) {
		this -> data [i] = payload [i + 4];
	}
}

uint32_t MessageDecoder::get_count(){
	return count;
}

bool MessageDecoder::has_next () const {
	return count > 0;
}

Message * MessageDecoder::next () {
	count --;

	Message * m = new Message ();
	offset += m -> from_byte_array(data + offset);

	return m;
}
