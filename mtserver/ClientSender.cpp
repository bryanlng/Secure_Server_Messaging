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

	//Read in name of the client, and check if it was created before
	std::string sender;
	bool nameCreatedAlready;
	std::string line;
	std::ifstream file("client_name.txt");
	if (file.is_open()) {
		getline(file, line);	//getline() grabs the string up to "\n"
		
		//If the user already set their name on a previous iteration, use it
		if (line.compare("")) {
			sender = line;
			nameCreatedAlready = true;
		}
		else {
			nameCreatedAlready = false;
		}
		file.close();
	}
	
	//Send update message, to check if there were messages that we missed 
	std::string time_message = formatMessage("", "", "::Timestamp::");
	stream->send(time_message.c_str(), time_message.size());

	
	//Take in user input, then send it to the server
	while (1) {
		//If the name wasn't created already, prompt the user to fill it out
		//Then, write it to the client_name.txt
		if (!nameCreatedAlready) {
			std::string name;
			std::cout << "Type in a name: ";
			std::getline(std::cin, name);

			std::ofstream n_file("client_name.txt", std::ofstream::app);		//app = append
			if (n_file.is_open()) {
				std::string nl = "\n";
				n_file << name;
				n_file << nl;
				n_file.close();
			}

			sender = name;
			nameCreatedAlready = true;	//so that we only ask for the name once
		}

		std::string message;
		std::cout << "Type in a message: ";
		std::getline(std::cin, message);
		std::cout << "Message that was typed: " << message << "\n";

		//If user wants to quit, exit the program
		if (!message.compare("quit")) {
			exit(-1);
		}

		//If the delimiter is part of the message, prevent the user
		//from sending the message, as it'll screw everything up
		if (message.find(":::::::") != -1) {
			std::cout << "Bad message, type another one" << std::endl;
		}
		else {
			std::string formatted = formatMessage(sender, message, ":::::::");
			std::cout << "Formatted message: " << formatted << std::endl;
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
			timestamp <delimiter> date_formatted <delimiter> message <delimiter> sender <delimiter>
		-Ex:
			1485328997:::::::Wed Jan 25 00:23:17 2017:::::::kkkk:::::::bryan:::::::

	Generating a timestamp:
	So a huge problem I ran into was that c++'s std::time(NULL) doesn't produce a value that's EVEN CLOSE
	to java's System.currentTimeMillis(). I realized this while making the Android client. It took me FOREVER
	to find a good c++ solution, but I ended up using this:
	http://en.cppreference.com/w/cpp/chrono/time_point/time_since_epoch
	Except when casting, change std::chrono::hours --> std::chrono::milliseconds
	
*/
std::string ClientSender::formatMessage(std::string sender, std::string message, std::string delimiter) {

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

		//1. Generate timestamp, then concatenate onto our string.
		//Timestamp = time from Unix Epoch, similar to Java's System.currentTimeMillis()
		std::chrono::time_point<std::chrono::system_clock> p2;
		p2 = std::chrono::system_clock::now();
		std::time_t today_time = std::chrono::system_clock::to_time_t(p2);
		long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(
			p2.time_since_epoch()).count();

		std::stringstream sstm;
		sstm << millis;
		result = sstm.str();
		result += delimiter;

		//2a. Generate current date, but formatted, then concatenate
		//Also, replace the pesky \n char (last char before the null) 
		//with a null character
		char* formatted = std::ctime(&today_time);
		string tmp(formatted);
		formatted[tmp.length() - 1] = '\0';

		string temp(formatted);
		result += temp;
		result += delimiter;

		//3. Add on message
		result += message;
		result += delimiter;

		//4. Add on name
		result += sender;
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