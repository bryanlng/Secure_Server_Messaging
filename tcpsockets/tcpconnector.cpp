/*
   TCPConnector.h

   TCPConnector class definition. TCPConnector provides methods to actively
   establish TCP/IP connections with a server.

   AKA, it provides the connect() method, in the schematic

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
   limitations under the License

   More stuff:
   http://stackoverflow.com/questions/6729366/what-is-the-difference-between-af-inet-and-pf-inet-in-socket-programming
   https://msdn.microsoft.com/en-us/library/zx63b042.aspx
   http://www.mkssoftware.com/docs/man3/select.3.asp

   Specifies a local or remote endpoint address to which to connect to a socket
	   struct sockaddr_in{
	   short sin_family;				//address family, must be AF_INET
	   unsigned short sin_port;			//IP port
	   struct in_addr sin_addr;			//IP address
	   char sin_zero[8];				//Padding added to make the struct the same size as SOCKADDR
   };

    struct timeval {
        long    tv_sec;         // seconds 
        long    tv_usec;        // microseconds
    };
   htons():
   converts the unsigned short integer hostshort from host byte order to network byte order.
*/

#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "tcpconnector.h"

/*
	Input:
		1. Server host name/IP address 
		2. Server port #

	Establishes a connection with the server
	Actually call

	

*/
TCPStream* TCPConnector::connect(const char* server, int port)
{
    struct sockaddr_in address;				

    memset (&address, 0, sizeof(address));			//set address to be all 0s
    address.sin_family = AF_INET;					//AF_INET refers to addresses from the internet	
    address.sin_port = htons(port);					//Set IP port to be TCP port on which the 

	/*
		If given DNS host name in server is bad --> then it must be an
		IP address already
		Thus, convert the IP address from text --> binary form
		using inet_pton()
	*/
    if (resolveHostName(server, &(address.sin_addr)) != 0 ) {
        inet_pton(PF_INET, server, &(address.sin_addr));        
    } 

    // Create and connect the socket, bail if we fail in either case
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket() failed");
        return NULL;
    }
    if (::connect(sd, (struct sockaddr*)&address, sizeof(address)) != 0) {
        perror("connect() failed");
        close(sd);
        return NULL;
    }
    return new TCPStream(sd, &address);
}

/*
	connect(), but this time, with a time limit specified by timeout
*/
TCPStream* TCPConnector::connect(const char* server, int port, int timeout)
{
    if (timeout == 0) return connect(server, port);
    
    struct sockaddr_in address;

    memset (&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);

	/*
		If given DNS host name in server is bad --> then it must be an
		IP address already
		Thus, convert the IP address from text --> binary form
		using inet_pton()
	*/
    if (resolveHostName(server, &(address.sin_addr)) != 0 ) {
        inet_pton(PF_INET, server, &(address.sin_addr));        
    }     


    long arg;
    fd_set sdset;
    struct timeval tv;
    socklen_t len;
    int result = -1, valopt, sd = socket(AF_INET, SOCK_STREAM, 0);
    
    // Bail if we fail to create the socket
    if (sd < 0) {
        perror("socket() failed");
        return NULL;
    }    

    // Set socket to non-blocking
    arg = fcntl(sd, F_GETFL, NULL);		//get the file access mode, status flags
    arg |= O_NONBLOCK;					// make arg be O_NONBLOCK
    fcntl(sd, F_SETFL, arg);			//Set the file status flags to be O_NONBLOCK, aka non-blocking
    
    // Connect with time limit
    string message;
    if ((result = ::connect(sd, (struct sockaddr *)&address, sizeof(address))) < 0) 
    {
		//Means that socket is nonblocking, and the connection can't be completed immediately
        if (errno == EINPROGRESS)
        {
			//same code as TCPStream's waitForReadEvent()
            tv.tv_sec = timeout;
            tv.tv_usec = 0;
            FD_ZERO(&sdset);			//make the file descriptor sdset have zero bits
            FD_SET(sd, &sdset);			//set the bit for the file descriptor
            int s = -1;

			//Wait for 1 fd from our file descriptor set sdset to be "ready"
            do {
                s = select(sd + 1, NULL, &sdset, NULL, &tv);	
            } while (s == -1 && errno == EINTR);

			/*
			   http://man7.org/linux/man-pages/man2/connect.2.html
				Read the SO_ERROR option at level SOL_SOCKET to determine whether
				connect() completed successfully (SO_ERROR is zero) or
				unsuccessfully (SO_ERROR is one of the usual error codes
				listed here, explaining the reason for the failure).
			*/
            if (s > 0)
            {
                len = sizeof(int);
                getsockopt(sd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &len);	
                if (valopt) {
                    fprintf(stderr, "connect() error %d - %s\n", valopt, strerror(valopt));
                }
                // connection established
                else result = 0;
            }
            else fprintf(stderr, "connect() timed out\n");
        }
        else fprintf(stderr, "connect() error %d - %s\n", errno, strerror(errno));
    }

    // Return socket to blocking mode 
    arg = fcntl(sd, F_GETFL, NULL);
    arg &= (~O_NONBLOCK);
    fcntl(sd, F_SETFL, arg);

    // Create stream object if connected
    if (result == -1) return NULL;
    return new TCPStream(sd, &address);
}

/*
	Converts a DNS host name --> IP address in network byte order, 
	by calling getaddrinfo()
	If host name isn't a valid DNS name --> must be an IP address already --> return -1		
	Else, return 0
*/
int TCPConnector::resolveHostName(const char* hostname, struct in_addr* addr) 
{
    struct addrinfo *res;
  

    int result = getaddrinfo (hostname, NULL, NULL, &res);
    if (result == 0) {
        memcpy(addr, &((struct sockaddr_in *) res->ai_addr)->sin_addr, sizeof(struct in_addr));
        freeaddrinfo(res);
    }
    return result;
}
