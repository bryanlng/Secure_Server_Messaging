#include "MessageHandler.h"
#define NEWLINE_ASCII 10

/*
	Constructor
	set_name() is a method from Thread
*/
MessageHandler::MessageHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n)
	: connections(connects), m_queue(queue)
{
	set_name(n);
}

/*
	Function:
	1. Consumer threads remove and grab a MessageItem from the message queue
	   If there's no MessageItems in the message queue (like in the beginning),
	   then the MessageHandler blocks.
	2. Then, "broadcast" the message to every Consumer thread that:
		  1) Currently have a connection (TCPStream* object)
		  2) Isn't the sender of the message
	   by calling each Consumer Thread's send_message()
*/
void* MessageHandler::run() {
	// Remove 1 item at a time and process it. Blocks if no items are 
	// available to process.
	for (int i = 0;; i++) {
		std::cout << thread_name() << ", loop " << i << " - waiting for item..." << std::endl;
		MessageItem* item = m_queue.remove();
		std::cout << thread_name() << ", loop " << i << " - got one item..." << std::endl;

		//Case 1: MessageItem is an update request
		if (item->isUpdateRequest()) {
			std::cout << "Incoming update request!!" << std::endl;

			//Part 1: Reading from timestamp.txt
			std::string latest_ts = readTimestampFile();
			std::cout << "Message from read(): " << latest_ts << std::endl;

			//If Timestamp and master log are NOT empty, check if the client is behind.
			//If the client is behind, send the messages it missed back to the client.
			if (latest_ts.compare("")) {
				std::cout << "NOT empty Master log and timestamp" << std::endl;
				//Convert timestamp into a long
				long latest_timestamp = atol(latest_ts.c_str());

				std::cout << "item->getTimeOfLastReceived(): " << item->getTimeOfLastReceived() << std::endl;
				std::cout << "latest_timestamp: " << latest_timestamp << std::endl;

				//Only send messages back to the sender if its timestamp is
				//behind the server's latest timestamp
				if (item->getTimeOfLastReceived() < latest_timestamp) {

					//Part 2: Reading from master log
					std::vector<std::string> messages;
					readMasterLog(messages, item->getTimeOfLastReceived());

					//Supporting fields for finding the ConnectionHandler* of the sender
					ConnectionHandler* client;
					string sender = item->getThreadID();
					bool senderFound = false;
					std::cout << "Sender of message: " << sender << std::endl;

					//Find the ConnectionHandler* of the sender and stop when we find it
					std::vector<ConnectionHandler*>::const_iterator c_iterator = connections.begin();
					while (!senderFound && c_iterator != connections.end()) {
						ConnectionHandler* connection = *c_iterator;
						std::cout << "Name of current connection: " << connection->thread_name() << std::endl;
						if (!sender.compare(connection->thread_name())) {
							client = connection;
							senderFound = true;
						}

						++c_iterator;
					}

					//Parse each string --> MessageItem, then send each message back to the client
					std::vector<std::string>::const_iterator m_iterator;
					for (m_iterator = messages.begin(); m_iterator != messages.end(); ++m_iterator) {
						std::string raw_message = *m_iterator;
						//std::cout << "Message to send back to client: " << raw_message << std::endl;

						MessageItem* message_item = new MessageItem(raw_message);
						client->send_message(message_item);

						delete message_item;
					}

				}
			}
			

		}

		//Else, Case 2: MessageItem contains an actual message, and needs to be requested
		else {
			//Broadcast message by relaying the MessageItem to each of the other connections
			std::vector<ConnectionHandler*>::const_iterator iterator;
			string sender = item->getThreadID();
			std::cout << "Sender of message: " << sender << std::endl;
			for (iterator = connections.begin(); iterator != connections.end(); ++iterator) {
				ConnectionHandler* connection = *iterator;
				std::cout << "Name of current connection: " << connection->thread_name() << std::endl;

				//Broadcast to everyone who has a connection AND is not the sender of the message
				if (connection->hasAConnection() && sender.compare(connection->thread_name())) {
					connection->send_message(item);
				}
			}

			//Update master log with the new message
			std::string message = item->getRawMessage();
			write("master_log.txt", message);

			//Update timestamp file for the most recent timestamp
			std::stringstream sstm;
			sstm << item->getTimestamp();
			std::string timestamp = sstm.str();
			write("timestamp.txt", timestamp);

		}
		

		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}

/*
	Reads the last line of client_timestamp.txt,
	then returns it

	This function was originally implemented in ThreadSafeFile 
	as a case in read(), but I moved it here.
*/
std::string MessageHandler::readTimestampFile() {
	//First, read in 1 line of input backwards, one char at a time
	//We'll know it's a next line when the second NL line feed (new line)
	//shows up, which has an ascii value of 10.
	std::string raw;
	char c;
	int num_new_lines = 0;
	bool still_one_line = true;
	int i = 1;

	std::ifstream file("timestamp.txt", std::ios::ate);
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

	file.close();

	return raw;
}

/*
	Given a timestamp ts, puts all messages [whose timestamp is greater than ts]
	into a vector of strings.

	This function was originally implemented in ThreadSafeFile as a case in read(),
	but I moved it here.
*/
void MessageHandler::readMasterLog(std::vector<std::string>& messages, long ts) {
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
			long curr_timestamp = std::strtol(token.c_str(), &sz, 10);

			//If the timestamp >= timestamp from the client, put the line
			//into the vector
			if (curr_timestamp >= ts) {
				messages.push_back(line);
			}

		}

		file.close();
	}
}

/*
	Given a timestamp ts, puts all messages [whose timestamp is greater than ts]
	into a vector of strings.

	This function automatically puts a newline at the end of each message, so 
	we don't have to do it manually outside of the function

	This function was originally implemented in ThreadSafeFile as write(), 
	but I moved it here.
*/
void MessageHandler::write(std::string filename, std::string item) {
	std::ofstream file(filename.c_str(), std::ofstream::app);		//app = append
	if (file.is_open()) {
		std::string nl = "\n";
		file << item;
		file << nl;
		file.close();
	}
}