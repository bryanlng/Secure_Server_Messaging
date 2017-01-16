#include "MessageHandler.h"


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
			//Open up timestamp.txt and read the timestamp of the most recent message of the chat

			//




			//Supporting fields for finding the ConnectionHandler* of the sender
			ConnectionHandler* client;
			string sender = item->getThreadID();
			bool senderFound = false;
			std::cout << "Sender of message: " << sender << std::endl;

			//Find the ConnectionHandler* of the sender, then stop when
			std::vector<ConnectionHandler*>::const_iterator iterator = connections.begin();
			while (!senderFound && iterator != connections.end()) {
				ConnectionHandler* connection = *iterator;
				std::cout << "Name of current connection: " << connection->thread_name() << std::endl;
				if (!sender.compare(connection->thread_name())) {
					client = connection;
					senderFound = true;
				}

				++iterator;
			}				

			//Send each message back to the client

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

			//Update the master log and the file for the most recent timestamp file

		}

		

		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}