#include "ClientReceiver.h"
#define MAX_MESSAGE_SIZE 25600

ClientReceiver::ClientReceiver(TCPStream* s) : stream(s){}

/*
	Receives incoming messages from the server

	Steps:
		-Extract the timestamp, date, and message.
		-Afterwards, update client_timestamp.txt with the extracted timestamp, so
		 that it has the most updated timestamp
		-Then, display the message
		-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>
			Ex: 1485288373:Tue Jan 24 13:06:13 2017:df:
*/
void* ClientReceiver::run() {

	int i = 1;
	int len;
	char input[MAX_MESSAGE_SIZE];

	//Receive messages
	while ((len = stream->receive(input, sizeof(input)) > 0)) {
		std::cout << "Iteration " << i << std::endl;
		////std::cout << "Raw message received from server: " << input << std::endl;
		printf("Raw message received from server: %s\n", input);
		std::string raw(input);

		//If message isn't "", then extract its contents and display the message.
		//A "" message means that the timestamp and master log were empty
		if (raw.compare("")) {

			//Fields for parsing the string
			int delimiter_pos = 0;
			int which_one = 0;
			std::string current_item;

			std::string delimiter = ":::::::";

			//Fields to extract
			long long timestamp;
			std::string date_formatted;
			std::string message;
			std::string sender;

			//Parsing the string. String is "eaten" along the way
			while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
				current_item = raw.substr(0, delimiter_pos);
				std::cout << "Regular message, Item found: " << current_item << std::endl;
				switch (which_one) {
				case 0:		//timestamp
					timestamp = atoll(current_item.c_str());
					break;
				case 1:		//formatted date
					date_formatted = current_item;
					break;
				case 2:		//actual message
					message = current_item;
					break;
				case 3:		//name of sender
					sender = current_item;
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
			std::cout << message << std::endl;
			std::cout << "From: " << sender << std::endl;
			std::cout << "Sent at: " << date_formatted << std::endl;

			//Append the message onto the client's log of messages
			write("client_log.txt", input);
		
			//"Erase" the char array, so our next message doesn't come in with bits of the previous message
			memset(input, 0, MAX_MESSAGE_SIZE);

			++i;
		}	

	}

	std::cout << "Connection cut off" << std::endl;


	//should never get here
	return NULL;
}

void ClientReceiver::write(std::string filename, std::string item) {
	std::ofstream file(filename.c_str(), std::ofstream::app);		//app = append
	if (file.is_open()) {
		std::string nl = "\n";
		file << item;
		file << nl;
		file.close();
	}
}

/*
	Destructor
*/
ClientReceiver::~ClientReceiver() {
	delete stream;
}