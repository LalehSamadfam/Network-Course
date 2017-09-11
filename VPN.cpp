#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sstream>
#include <unistd.h>

//#define VN 04
//#define N NULL
#define LOG(x) cerr << x << endl;
#define BUFFER_SIZE 300
//#define LOG(X) ;

using namespace std;

struct name {
	int16_t a;
	int32_t b;
}__attribute__((__packed__)) ;

struct request_packet {
	uint8_t vn;
	uint8_t cd;
	uint16_t port;
	uint32_t ip;
};

struct response_packet {
	uint8_t null;
	uint8_t status;
	uint16_t port;
	uint32_t ip;
};

struct client_info { // old handler_input
	int port;
	int ip;
	int sockfd;
};

struct pipe {
	int sender_fd;
	int receiver_fd;
};

string ip_to_string (unsigned int ip) {
	unsigned char octets [4];

	for (int i = 0;i < 4;i ++) {
		octets [i] = ip % 256;
		ip /= 256;
	}

	stringstream ss;
	string result;

	ss << int(octets[3]) << '.' << int(octets[2]) << '.' << int(octets[1]) << '.' << int(octets[0]);
	ss >> result;

	return result;
}

int connect_to_server (int ip, int port) {

	int sockfd = socket (AF_INET, SOCK_STREAM, 0);

	LOG ("Trying to connect to " << ip_to_string(ip) << ":" << port);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(ip);

	if (connect (sockfd, (sockaddr *) &addr, sizeof(addr)) < 0) {
		cout << "Could not connect to " << ip_to_string (ip) << ":" << port << endl;
		return -1;
	}
	cout << "Connection established with " << ip_to_string (ip) << ":" <<  port << endl ;
	return sockfd;
}

unsigned int array_to_ip (const unsigned char * array) {
	unsigned int ip = 0;
	for (int i = 0;i < 4;i ++) {
		unsigned int b = array [i];
		ip = (ip << 8) + b;
	}
	return ip;
}

int get_domain_ip (const char * domain) {
	int ip;
	hostent * server;

	LOG("Before DNS Query, looking for domain: " << domain);
	server = gethostbyname(domain);
	ip = array_to_ip((const unsigned char *) server -> h_addr);
	LOG("After DNS Query " << ip_to_string(ip));

	return ip;
}

int connect_to_server (const char * domain, int port) {
	int ip = get_domain_ip (domain);

	return connect_to_server (ip, port);
}

void relate (int sender_fd, int receiver_fd) {
	char * buffer = new char [BUFFER_SIZE];
	ssize_t len;
	LOG ("Relating is going to be started!");
	while ((len = recv (sender_fd, buffer, BUFFER_SIZE, 0)) > 0) {
		send (receiver_fd, buffer, len, MSG_NOSIGNAL);
	}
	LOG ("Relating ended!");
}

void * relate0 (void * arg) {
	struct pipe * p = (struct pipe *) arg;
	relate (p -> sender_fd, p -> receiver_fd);
	return NULL;
}

void send_response (int sockfd, int ip, int port, int status) {
	char buffer [8];
	response_packet * packet = (response_packet *) buffer;
	packet -> null = 0;
	packet -> status = status;
	packet -> port = htons (port);
	packet -> ip = htonl (ip);

	send (sockfd, buffer, 8, 0);
	LOG ("Response is sent to client!");
}

bool is_socks4a (int ip) {
	return (ip < 256) && (ip > 0);
}

void *handle_client(void * arg) {

	client_info * client = (client_info *)arg;

	//comunicate with client
	int client_fd = client -> sockfd;
	int client_ip = client -> ip;
	int client_port = client -> port;

	char * buffer = new char [BUFFER_SIZE];
	int size = recv(client_fd, buffer, BUFFER_SIZE, 0);

	request_packet * request = (request_packet *) buffer;

	if (request -> cd != 1) { // TODO improve this
		LOG ("bad request");
		return NULL;
	}

	int server_ip = ntohl(request -> ip);
	int server_port = ntohs(request -> port);
	cout << "The client from " << ip_to_string (client_ip) << ":" << client_port << " requested for " << ip_to_string(server_ip) << ":"<< server_port <<endl;

	//connect to a server
	int server_fd;
	if (is_socks4a (server_ip)){
		LOG ("SOCKS 4A");

		int i;
		int j;
		char domain [BUFFER_SIZE];
		for (i = sizeof (request);buffer [i] != 0;i ++)
			; // ignoring id
		i ++;

		for (j = 0; buffer [i] != 0;i ++, j ++) {
			domain [j] = buffer [i];
		}
		domain [j] = 0;
		server_ip = get_domain_ip(domain);
	}
	server_fd = connect_to_server(server_ip, server_port);
	send_response (client_fd, server_ip, server_port, 0x5A);

	// relating from client to server
	pthread_t thread;
	struct pipe * p = new struct pipe ();
	p -> sender_fd = client_fd;
	p -> receiver_fd = server_fd;
	pthread_create (&thread , 0, relate0, (void *) p);

	relate (server_fd, client_fd);
}


int main (int argc, char *argv[]){

	//    connect_to_server("www.google.com", 80);
	//    connect_to_server(array_to_ip(new unsigned char [4] {213,233,170,2}), 80);

	//config proxy as a server

	int port = atoi(argv[1]);
	int sd = socket (PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in cli_addr;
	socklen_t  cli_addrlen;

	memset (&cli_addr, 0, sizeof(sockaddr_in));
	cli_addr.sin_port = htons(port);
	cli_addr.sin_family = PF_INET;

	//bind socket
	if (bind(sd, (struct sockaddr *) &cli_addr, sizeof(cli_addr))){
		cerr << "error in bind\n";
		return -1;
	}

	//listen
	if (listen (sd, 5)){
		cerr << "error in listen \n";
		return -1;
	}

	//accept
	while(true){
		cli_addrlen = sizeof(cli_addr);

		struct client_info * ci = new struct client_info ();
		ci -> sockfd = accept (sd, (struct sockaddr *) &cli_addr, &cli_addrlen);
		ci -> ip = ntohl (((sockaddr_in) cli_addr).sin_addr.s_addr);
		ci -> port = ntohs (((sockaddr_in) cli_addr).sin_port);
		cout << "A client from " << ip_to_string (ci -> ip)  << ":" << ci -> port << " was connected\n";

		pthread_t tid;
		pthread_create(&tid , 0, handle_client, (void *) ci);
	}
	cerr << "connected !\n";

}
