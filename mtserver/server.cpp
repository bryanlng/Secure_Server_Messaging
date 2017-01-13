/*
   main.cpp

   Multithreaded work queue based example server in C++.
  
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

   (long unsigned int)self()
   //stream->send(input, len);
   //char input[25600];
   http://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
   http://stackoverflow.com/questions/347949/how-to-convert-a-stdstring-to-const-char-or-char
*/

/*
Previous stuff that is all included in ConnectionHandler.h, which MessageHandler includes
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <list>
#include "wqueue.h"
#include "MessageItem.h"
#include "tcpacceptor.h"
#include "ConnectionHandler.h"
*/


#include "thread.h"
#include <sstream>
#include "MessageHandler.h"

int main(int argc, char** argv)
{
    // Process command line arguments
    if ( argc < 3 || argc > 4 ) {
        printf("usage: %s <workers> <port> <ip>\n", argv[0]);
        exit(-1);
    }
    int workers = atoi(argv[1]);
    int port = atoi(argv[2]);
    string ip;
    if (argc == 4) { 
        ip = argv[3];
    }
 
    // Create the queues and consumer (worker) threads
	list<ConnectionHandler*> connections;
    wqueue<WorkItem*>  work_queue;			//work queue 1, manages the Consumer Threads
	wqueue<MessageItem*> message_queue;		//work queue 2, manages the actual messages

	// Create the sole Message Thread, which is responsible for broadcasting messages
	string message_id = "message_handler";
	MessageHandler* messenger = new MessageHandler(connections, message_queue, message_id);
	messenger->start();

	// Create the Consumer Threads, which take in and accept Connections
    for (int i = 0; i < workers; i++) {
		std::stringstream sstm;
		sstm << "thread" << i;
		std::string name = sstm.str();
        ConnectionHandler* handler = new ConnectionHandler(connections, work_queue, message_queue, name);
        if (!handler) {
            printf("Could not create ConnectionHandler %d\n", i);
            exit(1);
        } 
		connections.push_back(handler);
		std::cout << "New length of connections list: " << connections.size() << std::endl;
        handler->start();
    }
 
    // Create an acceptor then start listening for connections
    WorkItem* item;
    TCPAcceptor* connectionAcceptor;
    if (ip.length() > 0) {
        connectionAcceptor = new TCPAcceptor(port, (char*)ip.c_str());
    }
    else {
        connectionAcceptor = new TCPAcceptor(port);        
    }                                        
    if (!connectionAcceptor || connectionAcceptor->start() != 0) {
        printf("Could not create an connection acceptor\n");
        exit(1);
    }

    // Add a work item to the queue for each connection
    while (1) {
        TCPStream* connection = connectionAcceptor->accept(); 
        if (!connection) {
            printf("Could not accept a connection\n");
            continue;
        }
        item = new WorkItem(connection);
        if (!item) {
            printf("Could not create work item a connection\n");
            continue;
        }
		work_queue.add(item);
    }
 
    // Should never get here
    exit(0);
}
