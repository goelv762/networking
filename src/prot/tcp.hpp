#pragma once

#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <algorithm>

constexpr uint16_t NORMAL_PORT = 8080;

struct ClientInfo {
	int socket;
	std::string ip;
	std::string name;
};

class tcpSocket {
	protected:
	int port;
	int sock;
	sockaddr_in address;
	
	public:
	tcpSocket(in_addr_t givenAddress, uint16_t givenPort): port(givenPort) {
		sock = socket(AF_INET, SOCK_STREAM, 0);

		address = {
			// ipv4
			.sin_family = AF_INET,
			.sin_port = htons(port)
		};

		address.sin_addr.s_addr = givenAddress;
	}

	~tcpSocket() {
		close(sock);
	}
};

class Server : public tcpSocket {
	public:
	Server(in_addr_t givenAddress, uint16_t givenPort) : tcpSocket(givenAddress, givenPort) {
		int result = bind(sock, (struct sockaddr*)&address, sizeof(address));
		if (result < 0) {
			perror("bind failed");
		}

		if (listen(sock, 1) < 0) {
			perror("listen failed");
		}
	}

	ClientInfo gainClient() {
		struct sockaddr_in clientAddr;
		socklen_t clientSize = sizeof(clientAddr);
		int clientSocket = accept(sock, (struct sockaddr *)&clientAddr, &clientSize);
		clientSockets.push_back(clientSocket);

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

		std::cout << "[client] new client, IP: " << clientIP << std::endl;
		return {clientSocket, clientIP, clientIP};
	}

	bool recvMessage(ClientInfo& clientInfo) {
		char charBuffer[1024] = {0};
		int bytesReceived = recv(clientInfo.socket, charBuffer, sizeof(charBuffer), 0);


		if (bytesReceived > 0) {
			std::string buffer(charBuffer, bytesReceived);
			// null terminate
			buffer[bytesReceived] = '\0';

			if (buffer[0] == '/') {
				std::string newName = buffer.substr(1, buffer.length() - 1);
				std::cout << clientInfo.ip << " nick as " << newName << std::endl;
				clientInfo.name = newName;
			} else {
				std::string msg = "[" + clientInfo.name + "]: " + buffer;
				std::cout << msg << std::endl;
				for (int sock : clientSockets) {
					if (sock != clientInfo.socket) {
						send(sock, msg.c_str(), msg.size(), 0);
					}
				}
			}
		} else if (bytesReceived == 0) {
			return false;
		}

		return true;
	}

	void deleteClient(int clientSocket) {
		auto sock = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
		if (sock != clientSockets.end()) {
			clientSockets.erase(sock);
		}

		close(clientSocket);
	}

	private:
	std::vector<int> clientSockets;
};

class Client : public tcpSocket {
	public:
	Client(in_addr_t givenAddress, uint16_t givenPort) : tcpSocket(givenAddress, givenPort) {
		int result = connect(sock, (struct sockaddr*)&address, sizeof(address));
	}
	
	void sendMessage(std::string msg) {
		send(sock, msg.c_str(), msg.size(), 0);
	}

	bool recvMessage(std::string &str) {
		char buffer[1024] = {0};
		int bytes = recv(sock, buffer, sizeof(buffer), 0);
		if (bytes == 0) {
			return false;
		}

		str = buffer;
		return true;
	}
};
