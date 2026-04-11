#pragma once

#include <string>
#include <sys/types.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <net/if.h>


#include <iostream>
#include <fstream>
#include <sstream>

// loops in /proc/net/route for default desination (dst = 0)
inline std::string getIP() {
	std::ifstream routeFile("/proc/net/route");
	std::string line;
	std::string defaultDevice;
	while (getline(routeFile, line)) {
		std::istringstream lineSS(line);
		
		std::string device;
		lineSS >> device;

		if (device == "Iface") {
			// first line definition
			continue;
		}
		
		int dst;
		// not that safe but /proc/net/route should be consistent!
		lineSS >> dst;
		
		if (dst == 0) {
			defaultDevice = device;
			break;
		}
	}

	routeFile.close();

    ifaddrs* ifaddr;
    getifaddrs(&ifaddr);

	ifaddrs* current_ifa = ifaddr;

	std::string address;

	// traverse linked list
    while (current_ifa != nullptr) {
		// gets rid of non - ipv4 addresses
		if (current_ifa->ifa_addr->sa_family != AF_INET ||
			// skip interfaces that are down
			!(current_ifa->ifa_flags & IFF_UP) ||
			// skip loopback (127.0.0.1)
			current_ifa->ifa_flags & IFF_LOOPBACK) {

			current_ifa = current_ifa->ifa_next;
			continue;
		}
		
		char addrBuffer[1024];
		auto* addr = (struct sockaddr_in*)current_ifa->ifa_addr;
		inet_ntop(AF_INET, &addr->sin_addr, addrBuffer, INET_ADDRSTRLEN);

		std::string name = current_ifa->ifa_name;
		
		if (name == defaultDevice) {
			address = addrBuffer;
		}

		current_ifa = current_ifa->ifa_next;
    }

    freeifaddrs(ifaddr);

	return address;
}
