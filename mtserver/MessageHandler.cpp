#include "MessageHandler.h"


/*
	Constructor
	set_name() is a method from Thread
*/
MessageHandler::MessageHandler(list<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n)
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

		//Broadcast message by relaying the MessageItem to each of the other connections
		std::list<ConnectionHandler*>::const_iterator iterator;
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

		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}