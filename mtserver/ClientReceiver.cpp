#include "ClientReceiver.h"
#define MAX_MESSAGE_SIZE 25600

ClientReceiver::ClientReceiver(TCPStream* s) : stream(s) {}

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

	int len;
	char input[MAX_MESSAGE_SIZE];

	//Receive messages
	while ((len = stream->receive(input, MAX_MESSAGE_SIZE - 1) > 0)) {
		//std::cout << "Raw message received from server: " << input << std::endl;
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

			//Write the timestamp to client_timestamp.txt, with a newline at the beginning
			/*ofstream master_filestream;
			ThreadSafeFile* t_file = new ThreadSafeFile("client_timestamp.txt");

			std::stringstream sstm;
			sstm << timestamp;
			std::string ts = sstm.str();

			t_file->write(ts);

			delete t_file;*/

			//Display message
			std::cout << message << std::endl;
			std::cout << "Sent at: " << date_formatted << std::endl;

			//"Erase" the char array, so our next message comes in cleanly
			for (int i = 0; i < len; i++) {
				input[i] = '\0';
			}
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