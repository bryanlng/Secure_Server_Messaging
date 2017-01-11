/*
   client.cpp

   Test client for the tcpsockets classes. 

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
*/

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include "tcpconnector.h"

/*
	Generates the required format
	timestamp <delimiter> date_formatted <delimiter> message <delimiter>
*/
string formatMessage(string message, string delimiter) {
	string result;

	//1. Generate timestamp, then concatenate onto our string
	std::time_t timer = std::time(NULL);
	long millis = static_cast<long>(timer);
	std::stringstream sstm;
	sstm << millis;
	result = sstm.str();
	result += delimiter;

	//2. Generate current date, but formatted, then concatenate
	char* formatted = std::asctime(std::localtime(&timer));
	string temp(formatted);
	result += temp;
	result += delimiter;

	//3. Add on message
	result += message;
	result += delimiter;

	return result;
}

int main(int argc, char** argv)
{
	//Given: Ex: ./client 9999 localhost
	//Mine:  ./client 9999 localhost <message>
    if (argc != 4) {
        printf("usage: %s <port> <ip>\n", argv[0]);
        exit(1);
    }

    int len;
    string message;
	string delimiter = ":";
	string temp(argv[3]);
	string test = (temp, delimiter);
	std::cout << "Test: " << test << std::endl;
	string formatted;
    char line[256];
    TCPConnector* connector = new TCPConnector();
    TCPStream* stream = connector->connect(argv[2], atoi(argv[1]));
    if (stream) {
		message = std::string(argv[3]);
		//message = (message, delimiter);
		stream->send(message.c_str(), message.size());
		printf("sent - %s\n", message.c_str());
		len = stream->receive(line, sizeof(line));
		line[len] = '\0';
		printf("received - %s\n", line);
		sleep(1);
   }

   /* stream = connector->connect(argv[2], atoi(argv[1]));
    if (stream) {
        message = "Why is there air?";
        stream->send(message.c_str(), message.size());
        printf("sent - %s\n", message.c_str());
        len = stream->receive(line, sizeof(line));
        line[len] = NULL;
        printf("received - %s\n", line);
        delete stream;
    }
    exit(0);*/
}
