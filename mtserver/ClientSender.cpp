#include "ClientSender.h"
#define NEWLINE_ASCII 10

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

	//Send update message
	std::string time_message = formatMessage("", "::Timestamp::");
	stream->send(time_message.c_str(), time_message.size());

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
			std::string formatted = formatMessage(message, ":::::::");
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
		-Ex:
			199999999::Timestamp::

	Case 2: Regular message
		-Simply create the string in the format as specified below
			-Timestamp will be the current time, in milliseconds
			-date_formatted will the current time, formatted in
			-message will be the message
		-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>
		-Ex:
			1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::
*/
std::string ClientSender::formatMessage(std::string message, std::string delimiter) {

	//Case 1: Update message
	if (!delimiter.compare("::Timestamp::")) {

		//Read from client's timestamp file
		std::string latest_ts = readTimestampFile();
		std::cout << "client latest timestamp from read(): " << latest_ts << std::endl;

		//Extract the message, without the \n at the end
		std::string no_nl = latest_ts.substr(0, latest_ts.find("\n"));

		//Append on the delimiter
		return no_nl + delimiter;
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
	Reads the last line of client_timestamp.txt,
	then returns it

	This function was originally in ThreadSafeFile's read(),
	but I moved it here.
*/
std::string ClientSender::readTimestampFile() {
	//First, read in 1 line of input backwards, one char at a time
	//We'll know it's a next line when the second NL line feed (new line)
	//shows up, which has an ascii value of 10.
	std::string raw;
	char c;
	int num_new_lines = 0;
	bool still_one_line = true;
	int i = 1;

	std::ifstream file("client_timestamp.txt", std::ios::ate);
	std::streampos size = file.tellg();
	while (still_one_line && i < size + 1) {
		file.seekg(-i, std::ios::end);
		file.get(c);
		//printf("%c, ", c);
		//printf("int rep: %d\n", c);
		//If we encounter a newline char, increment
		if (c == NEWLINE_ASCII) {
			++num_new_lines;
		}

		//It's the next line, so stop
		if (num_new_lines == 2) {
			still_one_line = false;
		}

		//If we didn't encounter a new line
		else {
			raw += c;
		}

		++i;
	}

	//Then, reverse the string, so it's in the correct order
	std::reverse(raw.begin(), raw.end());

	std::cout << "read timestamp: " << raw << std::endl;
	
	//Close file
	file.close();

	return raw;
}



/*
	Destructor
*/
ClientSender::~ClientSender() {
	delete stream;
}