#include "ClientSender.h"

ClientSender::ClientSender(TCPStream* s) : stream(s) {}

/*
	First, send an update message to the server to see if
	the client is behind on messages. Then, perform 
	regular function
	
	Takes in user input forever
	Once input is present, format it, then send it 
	to the server 
*/
void* ClientSender::run() {

	////Send update message
	//std::string time_message = formatMessage("", "??");
	//stream->send(time_message.c_str(), time_message.size());

	//Take in user input, then send it to the server
	while (1) {
		std::string message;
		std::cout << "Type in a message: ";
		std::cin >> message;
		
		//If user wants to quit, exit the program
		if (!message.compare("quit")) {
			exit(-1);
		}

		else {
			std::string formatted = formatMessage(message, "::");
			//std::cout << "Formatted message: " << formatted << std::endl;
			stream->send(formatted.c_str(), formatted.size());
		}

		
	}

	//should never get here
	return NULL;
}

/*
	Given a regular message, generates a formatted version with metatdata included
	that can be parsed by the server 

	Case 1: Update message
		-Read the latest timestamp from the client's timestamp file, then 
		 append the delimiter to the end of it
		-timestamp is the timestamp of the last message received
		-Format:
			timestamp <special timestamp delimiter>

	Case 2: Regular message
		-Simply create the string in the format as specified below
			-Timestamp will be the current time, in milliseconds
			-date_formatted will the current time, formatted in
			-message will be the message
		-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>
*/
std::string ClientSender::formatMessage(std::string message, std::string delimiter) {

	//Case 1: Update message
	if (!delimiter.compare("??")) {

		//Read from client's timestamp file
		ofstream time_filestream;
		ThreadSafeFile* t_file = new ThreadSafeFile("client_timestamp.txt");
		std::vector<std::string> t_vector;
		t_file->read(t_vector, 0);

		//Get the string from the vector of strings
		//Since there should only be 1 string in the vector, we can use front()
		std::string latest_ts = t_vector.front();
		std::cout << "client latest timestamp from read(): " << latest_ts << std::endl;

		//Append on the delimiter
		return latest_ts + delimiter;
	}

	//Case 2: Regular message
	else {
		std::string result;

		//1. Generate timestamp, then concatenate onto our string
		std::time_t timer = std::time(NULL);
		long millis = static_cast<long>(timer);
		std::stringstream sstm;
		sstm << millis;
		result = sstm.str();
		result += delimiter;

		//2a. Generate current date, but formatted, then concatenate
		//Also, replace the pesky \n char (last char before the null) 
		//with a null character
		char* formatted = std::asctime(std::localtime(&timer));
		string tmp(formatted);
		formatted[tmp.length() - 1] = '\0';

		string temp(formatted);
		result += temp;
		result += delimiter;

		//3. Add on message
		result += message;
		result += delimiter;

		return result;
	}

	return "";
}



/*
	Destructor
*/
ClientSender::~ClientSender() {
	delete stream;
}