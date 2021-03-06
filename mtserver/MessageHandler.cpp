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
	// Remove 1 item at a time and process it. Blocks if no items are available to process.
	for (int i = 0;; i++) {
		MessageItem* item = m_queue.remove();

		//Broadcast message by relaying the MessageItem to each of the other connections
		std::vector<ConnectionHandler*>::const_iterator iterator;
		string sender = item->getThreadID();
		for (iterator = connections.begin(); iterator != connections.end(); ++iterator) {
			ConnectionHandler* connection = *iterator;

			//Broadcast to everyone who has a connection AND is not the sender of the message
			if (connection->hasAConnection() && sender.compare(connection->thread_name())) {
				connection->send_message(item);
			}
		}

		//Update master log with the new message
		std::string message = item->getRawMessage();
		write("master_log.txt", message);
	
		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}

/*
	Appends a line of data (all the data from a MessageItem*) to the end of 
	the specified file. This file will most always be the server's master log
*/
void MessageHandler::write(std::string filename, std::string line) {
	std::ofstream file(filename.c_str(), std::ofstream::app);		//app = append
	if (file.is_open()) {
		std::string nl = "\n";
		file << line;
		file << nl;
		file.close();
	}
}