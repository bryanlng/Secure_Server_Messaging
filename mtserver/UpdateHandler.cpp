#include "UpdateHandler.h"
#define NEWLINE_ASCII 10

/*
	Constructor
	set_name() is a method from Thread
*/
UpdateHandler::UpdateHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n)
	: connections(connects), m_queue(queue)
{
	set_name(n);
}

/*
	Function:
	1. Consumer threads remove and grab a MessageItem from the update queue
	   If there's no MessageItems in the message queue (like in the beginning),
	   then the UpdateHandler blocks.
	2. Then, compare the timestamp from the MessageItem to the server's most up to date timestamp, which is the timestamp of the latest (last) line in master_log.txt
		1) If the server's master log isn't empty
			If the receieved timestamp < server's latest timestamp
				1) Find the ConnectionHandler* that represents the sender (who we're going to send the missed messages back to)
				2) Read all messages from the master log whose timestamp > received timestamp
				3) Send all messages back to the sender
		2) Else, do nothing
		
*/
void* UpdateHandler::run() {
	// Remove 1 item at a time and process it. Blocks if no items are available to process.
	for (int i = 0;; i++) {
		MessageItem* item = m_queue.remove();

		//If the master log is NOT empty, check if the client is behind.
		std::string latest_ts = readTimestamp();
		if (latest_ts.compare("")) {

			//Get the server's latest timestamp
			long long latest_timestamp = atoll(latest_ts.c_str());

			//If the client is behind, send the messages it missed back to the client.
			if (item->getTimeOfLastReceived() < latest_timestamp) {

				//Part 2: Reading from master log
				//Find the ConnectionHandler* of the sender and stop when we find it
				ConnectionHandler* client;
				string sender = item->getThreadID();
				bool senderFound = false;

				std::vector<ConnectionHandler*>::const_iterator c_iterator = connections.begin();
				while (!senderFound && c_iterator != connections.end()) {
					ConnectionHandler* connection = *c_iterator;
					if (!sender.compare(connection->thread_name())) {
						client = connection;
						senderFound = true;
					}

					++c_iterator;
				}

				//Read all messages from the master log whose timestamp is greater than the received timestamp
				std::vector<std::string> messages;
				readMasterLog(messages, item->getTimeOfLastReceived());

				//Parse each message into a MessageItem, then send each message back to the client
				std::vector<std::string>::const_iterator m_iterator;
				for (m_iterator = messages.begin(); m_iterator != messages.end(); ++m_iterator) {
					std::string raw_message = *m_iterator;
					MessageItem* message_item = new MessageItem(raw_message);
					client->send_message(message_item);
					delete message_item;
				}

			} 
		}
	
		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}

/*
	Reads the last line of master_log.txt, extracts the timestamp part out of it, 
	then returns the timestamp in the form of a string
*/
std::string UpdateHandler::readTimestamp() {
	std::string raw;
	char c;
	int num_new_lines = 0;
	bool still_one_line = true;
	int i = 1;

	std::ifstream file("master_log.txt", std::ios::ate);
	std::streampos size = file.tellg();
	while (still_one_line && i < size + 1) {
		file.seekg(-i, std::ios::end);
		file.get(c);

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

	//std::cout << "last line: " << raw << std::endl;
	std::string timestamp = extractTimestamp(raw);
	//std::cout << "extracted timestamp: " << timestamp << std::endl;
	return timestamp;
}

/*
	Given a timestamp ts, puts all messages [whose timestamp is greater than ts]
	into a vector of strings.
*/
void UpdateHandler::readMasterLog(std::vector<std::string>& messages, long long ts) {
	std::string delimiter = ":::::::";
	std::string line;
	std::ifstream file("master_log.txt");
	if (file.is_open()) {
		while (getline(file, line)) {	////getline() grabs the string up to "\n"
			//std::cout << "Masterlog current line: " << line << std::endl;

			//Extract the current timestamp out of the message
			int delimiter_pos = line.find(delimiter);
			std::string token = line.substr(0, delimiter_pos);
			char* sz;   // alias of size_t
			long long curr_timestamp = std::strtol(token.c_str(), &sz, 10);

			//If the timestamp >= timestamp from the client, put the line
			//into the vector
			if (curr_timestamp >= ts) {
				messages.push_back(line);
			}

		}

		file.close();
	}
}


std::string UpdateHandler::extractTimestamp(std::string line) {
	std::string delimiter = ":::::::";
	int delimiter_pos = line.find(delimiter);
	return line.substr(0, delimiter_pos);
}