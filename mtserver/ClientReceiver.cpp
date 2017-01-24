#include "ClientReceiver.h"
#define MAX_MESSAGE_SIZE 25600

ClientReceiver::ClientReceiver(TCPStream* s) : stream(s) {}

/*
	Receives incoming messages from the server
	There are 2 types of messages that the client receives:
	Case 1: Update message
		-Extract the timestamp out of the string
		-Write the timestamp to client_timestamp.txt
		-Format:
			timestamp <special timestamp delimiter>
			Ex: 12329312381??

	Case 2: Regular message
		-Extract the timestamp, date, and message. Then display it
		-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>
			Ex: 1485288373:Tue Jan 24 13:06:13 2017:df:
*/
void* ClientReceiver::run() {

	int len;
	char input[MAX_MESSAGE_SIZE];

	//Receive messages
	while ((len = stream->receive(input, MAX_MESSAGE_SIZE - 1) > 0)) {
		//std::cout << "Raw message received from server: " << input << std::endl;
		printf("Raw message received from server: %s\n", input);
		string raw(input);
		
		//Fields for parsing the string
		int delimiter_pos = 0;
		int which_one = 0;
		string current_item;

		//First check if the message is the "timestamp", aka update message
		string delimiter = "??";	//"::Timestamp!!!!!::";
		if ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
			current_item = raw.substr(0, delimiter_pos);
			std::cout << "Timestamp message, Item found: " << current_item << std::endl;
			long time_of_last_received = atol(current_item.c_str());

			//Write 
		}

		//Else, it's a regular message
		else {
			string delimiter = ":";
			
			//Fields to extract
			long timestamp;
			string date_formatted;
			string message;
		}

	}


	//should never get here
	return NULL;
}

/*
	Destructor
*/
ClientReceiver::~ClientReceiver() {
	delete stream;
}