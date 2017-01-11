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
*/

#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include "thread.h"
#include "wqueue.h"
#include "tcpacceptor.h"
#define MAX_MESSAGE_SIZE 25600
 
class WorkItem
{
	private:
		TCPStream* m_stream;
 
	public:
		WorkItem(TCPStream* stream) : m_stream(stream) {}
		~WorkItem() { delete m_stream; }
 
		TCPStream* getStream() { return m_stream; }
};


class MessageItem
{
	// Delimiter:  ::&$*@^$^$(@(::
	/*
		Format of message:
		timestamp <delimiter> date_formatted <delimiter> message <delimiter>
	*/
	private:
		string raw;
		long timestamp;
		string date_formatted;
		string message;

	public:
		MessageItem(std::string full, long time, std::string date, std::string mes) :
			raw(full), timestamp(time), date_formatted(date), message(mes) {}

};



class ConnectionHandler : public Thread
{
    wqueue<WorkItem*>& w_queue;
	list<ConnectionHandler*>& all_connections;
	wqueue<MessageItem*>& m_queue;
 
  public:
    ConnectionHandler(wqueue<WorkItem*>& queue, list<ConnectionHandler*>& connections, wqueue<MessageItem*>& message_queue, std::string n)
		: w_queue(queue), all_connections(connections), m_queue(message_queue)
	{
		set_name(n);
	}
 
    void* run() {
        // Remove 1 item at a time and process it. Blocks if no items are 
        // available to process.
        for (int i = 0;; i++) {
			std::cout << thread_name() << ", loop " << i << " - waiting for item..." << std::endl;
            WorkItem* item = w_queue.remove();
			std::cout << thread_name() << ", loop " << i << " - got one item..." << std::endl;
            TCPStream* stream = item->getStream();

            // 1. Parse contents into their fields ==> then put into a MessageItem object
			// Delimiter will be: ::&$*@^$^$(@(::
			// Delimiter will not be seen by the user

			//Fields for receiving the message
			char* input;
			input = (char*)malloc(sizeof(char) * MAX_MESSAGE_SIZE);
            int len;

			//Fields to put inside the Message item
			string raw_message;
			long timestamp;
			string date_formatted;
			string message;

			MessageItem* message_item;
			while ((len = stream->receive(input, MAX_MESSAGE_SIZE-1) > 0 )){
				std::cout << "Raw message received from client: " << input << std::endl;
				string raw(input);
				raw_message = raw;

				//Fields for parsing the string
				string delimiter = "::&$*@^$^$(@(::";
				int delimiter_pos = 0;
				int which_one = 0;
				string current_item;

				//Parsing the string. String is "eaten" along the way
				while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
					current_item = raw.substr(0, delimiter_pos);
					std::cout << "Item found: " << current_item << std::endl;
					switch (which_one) {
						case 0:		//timestamp
							timestamp = atol(current_item.c_str());
							break;
						case 1:		//formatted date
							date_formatted = current_item;
							break;
						case 2:		//actual message
							message = current_item;
							break;
						default:
							std::cout << "Should not get here" << std::endl;
							break;
					}
					
					//update fields
					int next_index = delimiter_pos + delimiter.length();
					raw = raw.substr(next_index, raw.size());
					++which_one;
				}

				//Create a new message item and add it to the message queue
				message_item = new MessageItem(raw_message, timestamp, date_formatted, message);
				m_queue.add(message_item);
            }
			free(input);
            delete item; 

        }

        // Should never get here
        return NULL;
    }

	/*void send_message(TCPStream* stream, ) {

	}*/
};

class MessageHandler : public Thread
{
	list<ConnectionHandler*>& all_connections;
	wqueue<MessgeItem*>& m_queue;

public:
	ConnectionHandler(list<ConnectionHandler*>& connections, wqueue<MessgeItem*>& queue, std::string n)
		: all_connections(connections), m_queue(queue)
	{
		set_name(n);
	}

	void* run() {
		// Remove 1 item at a time and process it. Blocks if no items are 
		// available to process.
		for (int i = 0;; i++) {
			std::cout << thread_name() << ", loop " << i << " - waiting for item..." << std::endl;
			MessgeItem* item = m_queue.remove();
			std::cout << thread_name() << ", loop " << i << " - got one item..." << std::endl;
			TCPStream* stream = item->getStream();

			
		}

		// Should never get here
		return NULL;
	}

	/*void send_message(TCPStream* stream, ) {

	}*/
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
	list<ConnectionHandler*> all_connections;
    wqueue<WorkItem*>  work_queue;			//work queue 1, manages the Consumer Threads
	wqueue<MessageItem*> message_queue;		//work queue 2, manages the actual messages
    for (int i = 0; i < workers; i++) {
		std::stringstream sstm;
		sstm << "thread" << i;
		std::string name = sstm.str();
        ConnectionHandler* handler = new ConnectionHandler(work_queue, all_connections, message_queue, name);
        if (!handler) {
            printf("Could not create ConnectionHandler %d\n", i);
            exit(1);
        } 
		all_connections.push_back(handler);
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
