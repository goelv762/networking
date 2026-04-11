#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <net/if.h>
#include <thread>
#include <vector>

#include "prot/tcp.hpp"
#include "prot/udp.hpp"
#include "prot/ip.hpp"

void handleClient(ClientInfo client, Server* server) {
	bool isValid = true;
    while (isValid) {
        isValid = server->recvMessage(client);
    }

	// TODO: broadcast this to all other clients
	std::cout << "[" << client.name << "] disconnected." << std::endl;
	server->deleteClient(client.socket);
}

void routeClient(RecivceBroadcast* rb, std::string serverIP) {
	while (true) {
		sockaddr_in client = rb->recive();
		std::cout << "[client] " << inet_ntoa(client.sin_addr) << 
					 " routing to [server] " << serverIP << std::endl;

		rb->respond(client);
	}
}

int main() {
	Server s(inet_addr(getIP().c_str()), NORMAL_PORT);

	RecivceBroadcast rb(NORMAL_PORT);
	std::string ip = getIP();

	std::thread rbThread(routeClient, &rb, ip);
	
	std::vector<std::thread> clientThreads;
	 
	while (true) {
		ClientInfo ci = s.gainClient();
	 	clientThreads.emplace_back(handleClient, ci, &s);
	}
}
