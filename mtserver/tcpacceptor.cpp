/*
	TCPAcceptor.cpp
	TCPAcceptor class definition. TCPAcceptor provides methods to passively
	establish TCP/IP connections with clients.
	Includes member variables for the LISTENING socket descriptor
	------------------------------------------
	Copyright (c) 2013 Vic Hargrave

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
	http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.

	My notes:
	http://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
	AF_INET = IP addresses from the internet
	PF_INET = Anything from the protocol family, usually just sockets/ports
	Specifies a local or remote endpoint address to which to connect to a socket
	struct sockaddr_in{
		short sin_family;				//address family, must be AF_INET
		unsigned short sin_port;			//IP port
		struct in_addr sin_addr;			//IP address
		char sin_zero[8];				//Padding added to make the struct the same size as SOCKADDR
	};
*/

#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "tcpacceptor.h"

/*
	Constructor
	m_lsd value of 0 ==> means that listening socket hasn't been created yet
*/
TCPAcceptor::TCPAcceptor(int port, const char* address)
	: m_lsd(0), m_port(port), m_address(address), m_listening(false) {}

/*
	Destructor
	If listening socket has already been created, close it
*/
TCPAcceptor::~TCPAcceptor()
{
	if (m_lsd > 0) {
		close(m_lsd);
	}
}

/*
	Starts listening for connections
	Calls bind() and listen()
*/
int TCPAcceptor::start()
{
	//If a listening socket already exists, no need to create another one, so just return 0
	if (m_listening == true) {
		return 0;
	}

	//Else, create a socket using socket(). 
	//More details here http://man7.org/linux/man-pages/man2/socket.2.html
	m_lsd = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in address;

	//Set address's protocol family, port#
	memset(&address, 0, sizeof(address));
	address.sin_family = PF_INET;
	address.sin_port = htons(m_port);

	//If address field has already been set, use inet_pton() to convert it 
	//into a numerical IP address in network byte order
	if (m_address.size() > 0) {
		inet_pton(PF_INET, m_address.c_str(), &(address.sin_addr));
	}

	//Else, inet_pton() failed ==> thus, the socket listening address is set
	//to any IP address.
	else {
		address.sin_addr.s_addr = INADDR_ANY;
	}

	//Make it possible to IMMEDIATELY reuse a listening port, by setting
	//the socket option SO_REUSEADDR to a certain value
	int optval = 1;
	setsockopt(m_lsd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	//Bind the listening socket address --> socket descriptor using bind()
	int result = bind(m_lsd, (struct sockaddr*)&address, sizeof(address));
	if (result != 0) {
		perror("bind() failed");
		return result;
	}

	//Turn on server listening using listen().
	//If listen() succeeded, set the listening flag to be true
	result = listen(m_lsd, 5);
	if (result != 0) {
		perror("listen() failed");
		return result;
	}
	m_listening = true;
	return result;
}

/*
	Accepts connections from clients
*/
TCPStream* TCPAcceptor::accept()
{
	//If the socket is not in the listening state, then how could we possibly accept connections?
	//Return NULL
	if (m_listening == false) {
		return NULL;
	}

	//Call the socket function's accept(), not ours. accept() will block
	struct sockaddr_in address;
	socklen_t len = sizeof(address);
	memset(&address, 0, sizeof(address));
	int sd = ::accept(m_lsd, (struct sockaddr*)&address, &len);

	//If there was an error, return NULL.
	if (sd < 0) {
		perror("accept() failed");
		return NULL;
	}
	return new TCPStream(sd, &address);
}