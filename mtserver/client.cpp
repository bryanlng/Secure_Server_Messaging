/*
   client.cpp

   Class to represent a user. 
   A user can send and receive messages.

   What the user sees:
   To start the client app, do ./client port ip (ex: ./client 9999 localhost)
   This is pretty barebones, as this is going to be a basis for the Android
   client, which will look much nicer than this

   What's actually going on:
   2 Threads are running at the same time:
		1) ClientSender
		2) ClientReceiver

   1. ClientSender:
		-First things first, client sends an "update" message to the server,
		 to see if its behind
		-Then, ClientSender sits there and waits for input from stdout
		-Once it has input, it sends it to the server, which broadcasts the 
		 message to all other clients
		-Client can send 2 types of messages:
			1) Update message:
			    -Ask the server if client is behind, and if we are, send us all
				 the messages that we didn't get 
				-Message contains client's timestamp of the last message it 
				 received
				 -timestamp <special timestamp delimiter>

			2) Regular message:
				-timestamp <delimiter> date_formatted <delimiter> message <delimiter>

   2. ClientReceiver:
		-Sits there and waits for messages to come in
		-2 types of messages can come in:
			1) Update message:
				-The updated, most recent timestamp from the server
				-timestamp <special timestamp delimiter>

			2) Regular message:
				-timestamp <delimiter> date_formatted <delimiter> message <delimiter>


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

   http://en.cppreference.com/w/cpp/chrono/c/time
   http://en.cppreference.com/w/cpp/chrono/c/gmtime
   http://stackoverflow.com/questions/1662909/undefined-reference-to-pthread-create-in-linux
*/

#include "ClientTimestampFiller.h"
#include "tcpconnector.h"
#include <iostream>
#include <sstream>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define MAX_MESSAGE_SIZE 25600

int main(int argc, char** argv)
{
	//Given example:	 ./client 9999 localhost
	//Testing purposes:  ./client 9999 localhost <message>
    if (argc != 3) {
        printf("usage: %s <port> <ip>\n", argv[0]);
        exit(1);
    }

	//If the client's timestamp file hasn't been created yet, 
	//create it, and give it the value 0, so that it MUST pull
	//from the server
	ofstream t_file;
	if (!std::ifstream("client_timestamp.txt")) {
		std::cout << "Created client_timestamp.txt b/c it didn't exist" << std::endl;
		t_file.open("client_timestamp.txt");

		std::string nl = "\n";
		long long millis = 0;
		t_file << millis;
		t_file << nl;

		t_file.close();
	}

	//If the client's name file hasn't been created yet, create it
	ofstream n_file;
	if (!std::ifstream("client_name.txt")) {
		std::cout << "Created client_name.txt b/c it didn't exist" << std::endl;
		n_file.open("client_name.txt");
		n_file.close();
	}

	//Establish connection with server
    TCPConnector* connector = new TCPConnector();
    TCPStream* stream = connector->connect(argv[2], atoi(argv[1]));

	//If there's a connection, create the 2 Threads to send and receive messages
	if (stream) {
		wqueue<long>  m_queue;
		std::string name = "timestamp_filler";
		ClientTimestampFiller* timestamp_filler = new ClientTimestampFiller(m_queue,name);
		timestamp_filler->start();

		ClientSender* sender = new ClientSender(stream);
		sender->start();

		ClientReceiver* receiver = new ClientReceiver(stream, m_queue);
		receiver->start();



		sleep(UINT_MAX);		//Need to get this thread to not be active
								//unfortunately jank code, but it works				
   }

}
