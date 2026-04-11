#pragma once

#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>

constexpr uint16_t DISCOVERY_PORT = 5000;

struct ServerInfo {
	in_addr_t ip;
	uint16_t port;
	std::string name;
};

class udpSocket {
	protected:
	int port;
	int sock;
	sockaddr_in address;
	
	public:
	udpSocket(in_addr_t givenAddress, int givenPort): port(givenPort) {
		sock = socket(AF_INET, SOCK_DGRAM, 0);

		address = {
			// ipv4
			.sin_family = AF_INET,
			.sin_port = htons(port)
		};

		address.sin_addr.s_addr = givenAddress;
	}

	~udpSocket() {
		close(sock);
	}
};

class Broadcast : public udpSocket {
	public:
	Broadcast() : udpSocket(INADDR_BROADCAST, DISCOVERY_PORT) {
		int broadcastEnable = 1;
		setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	}
	
	// broadcast to any servers that may be listening
	void cast() {
		const char* msg = "DISCOVER";
		sendto(sock, msg, strlen(msg), 0, (sockaddr*)&address, sizeof(address));
	}

	ServerInfo recive() {
		char buffer[1024];
		sockaddr_in sender{};
		socklen_t senderLen = sizeof(sender);
	
		int len = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderLen);

		buffer[len] = '\0';

		std::string bufferStr = buffer;

		int colonPosition = bufferStr.find(":");

		ServerInfo result = {
			.ip = sender.sin_addr.s_addr,
			.port = static_cast<uint16_t>(stoi(bufferStr.substr(colonPosition + 1))),
			.name = bufferStr.substr(0, colonPosition)
		};

		return result;
	}
};

class RecivceBroadcast : public udpSocket {
	public:
	int tcpPort;
	RecivceBroadcast(uint16_t tcpPort) : udpSocket(INADDR_ANY, DISCOVERY_PORT), tcpPort(tcpPort) {
		int result = bind(sock, (sockaddr*)&address, sizeof(address));
	}

	// get broadcast message from client
	sockaddr_in recive() {
		char buffer[1024];
		sockaddr_in sender{};
		socklen_t senderLen = sizeof(sender);
	
		int len = recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&sender, &senderLen);

		buffer[len] = '\0';
		return sender;
	}

	void respond(sockaddr_in client) {
		std::string reply = "TestServer:" + std::to_string(tcpPort);
		sendto(sock, reply.c_str(), strlen(reply.c_str()), 0, (sockaddr*)&client, sizeof(client));
	}
};
