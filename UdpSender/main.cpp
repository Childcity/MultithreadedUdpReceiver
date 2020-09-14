#include "../packing.hpp"
#include "../Common/Utils.h"

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>

#define SERVER_PORT "6333"
#define SERVER_ADR "localhost"

int main(int argc, char *argv[])
{
	int sockfd {};
	struct addrinfo hints{}, *servinfo, *p;
	int rv;

	ZeroMem(&hints, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((rv = getaddrinfo(SERVER_ADR, SERVER_PORT, &hints, &servinfo)) != 0) {
		std::cerr << "Error from getaddrinfo: " << gai_strerror(rv) << std::endl;
		return 1;
	}

	std::cout << "IP addresses for: " << SERVER_ADR << "\n"
			  << Utils::PrintIpAddresses(servinfo) << std::endl;

	// loop through all the results and make a socket
	for(p = servinfo; p != nullptr; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Error from socket");
			continue;
		}

		break;
	}

	if (p == nullptr) {
		std::cerr << "Error: failed to create socket" << std::endl;
		return 1;
	}

	struct addrinfo *serverAddr = p;

	ssize_t numBytes;
	auto s = "Hello !";
	while (true) {
		if ((numBytes = Utils::SendAll(sockfd, s, strlen(s), serverAddr)) == -1) {
			perror("talker: SendAll");
			exit(1);
		}
		std::cout << "talker: sent " << numBytes << " bytes to " <<SERVER_ADR << ":" << SERVER_PORT << std::endl;
		sleep(1);
	}


	freeaddrinfo(servinfo);
	close(sockfd);

	return 0;
}