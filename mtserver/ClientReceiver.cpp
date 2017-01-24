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
		std::string raw(input);
		
		//Fields for parsing the string
		int delimiter_pos = 0;
		int which_one = 0;
		std::string current_item;

		//First check if the message is the "timestamp", aka update message
		string delimiter = "??";	//"::Timestamp!!!!!::";
		if ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
			current_item = raw.substr(0, delimiter_pos);
			std::cout << "Timestamp message, Item found: " << current_item << std::endl;
			long time_of_last_received = atol(current_item.c_str());

			//Write the timestamp to client_timestamp.txt, with a newline at the beginning
			ofstream master_filestream;
			ThreadSafeFile* t_file = new ThreadSafeFile("client_timestamp.txt");

			std::string nl = "\n";
			std::stringstream sstm;
			sstm << time_of_last_received << nl;
			std::string timestamp = sstm.str();
			
			t_file->write(timestamp);
			
			delete t_file;
		}

		//Else, it's a regular message
		else {
			std::string delimiter = "::"; //"::&$*@^$^$(@(::";
			
			//Fields to extract
			long timestamp;
			std::string date_formatted;
			std::string message;

			//Parsing the string. String is "eaten" along the way
			while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
				current_item = raw.substr(0, delimiter_pos);
				std::cout << "Regular message, Item found: " << current_item << std::endl;
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
						std::cout << "Field is not used" << std::endl;
						break;
				}

				//update fields
				int next_index = delimiter_pos + delimiter.length();
				raw = raw.substr(next_index, raw.size());
				++which_one;
			}

			//Display message
			std::cout << "Incoming message: " << std::endl;
			std::cout << message << std::endl;
			std::cout << "Sent at: " << date_formatted << std::endl;
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