#include "thread.h"
#include "tcpacceptor.h"
#include "wqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>

/*
	Thread that encapsulates the functionality to send messages
	Prompts the user for input, then (behind the scenes) formats the message,
	then sends it to the server.
	
	http://stackoverflow.com/questions/10486116/what-does-this-gcc-error-relocation-truncated-to-fit-mean
	http://stackoverflow.com/questions/39931468/what-does-mean-when-used-in-a-g-argument
*/
class ClientSender : public Thread {
	private:
		TCPStream* stream;

	public:
		ClientSender(TCPStream* s);
		void* run();
		std::string formatMessage(std::string sender, std::string message, std::string delimiter);
		std::string readTimestampFile();
		~ClientSender();

};