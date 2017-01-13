#include "MessageHandler.h"
MessageHandler::MessageHandler(list<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n)
	: connections(connects), m_queue(queue)
{
	set_name(n);
}

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

		delete item;

	}

	// Should never get here
	return NULL;
}