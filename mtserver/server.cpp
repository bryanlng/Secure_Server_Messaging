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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include "thread.h"
#include "wqueue.h"
#include "tcpacceptor.h"
 
class WorkItem
{
    TCPStream* m_stream;
 
  public:
    WorkItem(TCPStream* stream) : m_stream(stream) {}
    ~WorkItem() { delete m_stream; }
 
    TCPStream* getStream() { return m_stream; }
};

class ConnectionHandler : public Thread
{
    wqueue<WorkItem*>& m_queue;
 
  public:
    ConnectionHandler(wqueue<WorkItem*>& queue) : m_queue(queue) {}
 
    void* run() {
        // Remove 1 item at a time and process it. Blocks if no items are 
        // available to process.
        for (int i = 0;; i++) {
			std::cout << thread_name() << ", loop " << i << " - waiting for item..." << std::endl;
            WorkItem* item = m_queue.remove();
			std::cout << thread_name() << ", loop " << i << " - got one item..." << std::endl;
            TCPStream* stream = item->getStream();

            // Echo messages back the client until the connection is 
            // closed
            char input[256];
            int len;
            while ((len = stream->receive(input, sizeof(input)-1)) > 0 ){
                input[len] = NULL;
                stream->send(input, len);
				std::cout << thread_name() << ", echoed " << input << " back to the client..." << std::endl;
            }
            delete item; 
        }

        // Should never get here
        return NULL;
    }
};

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
 
    // Create the queue and consumer (worker) threads
    wqueue<WorkItem*>  queue;
    for (int i = 0; i < workers; i++) {
		std::stringstream sstm;
		sstm << "thread" << i;
		std::string name = sstm.str();
        ConnectionHandler* handler = new ConnectionHandler(queue);
		handler->set_name(name);
        if (!handler) {
            printf("Could not create ConnectionHandler %d\n", i);
            exit(1);
        } 
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
        queue.add(item);
    }
 
    // Should never get here
    exit(0);
}
