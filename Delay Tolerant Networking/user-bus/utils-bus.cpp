#include "utils-bus.h"

using namespace std;

void print_scan(string network){
	cout << "City with IP" << network << "is in range" << endl;
}

void print_sent_message_num(int n, ip_t ip){
	cout << n << " messages are sent to " << ip << endl;
}
