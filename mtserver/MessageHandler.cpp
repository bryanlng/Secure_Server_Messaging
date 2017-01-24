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
			std::cout << "Incoming update request!!" << std::endl;

			//Part 1: Reading from timestamp.txt
			//Open up timestamp.txt and read the timestamp of the most recent message of the chat
			ofstream time_filestream;
			ThreadSafeFile* t_file = new ThreadSafeFile("timestamp.txt");
			std::vector<std::string> t_vector;
			t_file->read(t_vector, 0);

			//Get the string from the vector of strings
			//Since there should only be 1 string in the vector, we can use front()
			std::string latest_ts = t_vector.front();
			std::cout << "Message from read(): " << latest_ts << std::endl;

			//Convert timestamp into a long
			long latest_timestamp = atol(latest_ts.c_str());					
			
			//Only send messages back to the sender if its timestamp is
			//behind the server's latest timestamp
			if (item->getTimeOfLastReceived() < latest_timestamp) {
				//Part 2: Reading from master log
				ofstream master_filestream;
				ThreadSafeFile* m_file = new ThreadSafeFile("master_log.txt");
				std::vector<std::string> messages;
				m_file->read(messages, item->getTimeOfLastReceived());

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

				//Send the updated timestamp back to the client
				MessageItem* time_message = new MessageItem(latest_timestamp);
				client->send_message(time_message);

				//Free fields
				delete time_message;
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

			//Update master log with the new message, with a 
			//newline at the beginning
			ofstream master_filestream;
			ThreadSafeFile* m_file = new ThreadSafeFile("master_log.txt");
			std::string nl = "\n";
			std::string message = item->getRawMessage();
			message += nl;
			m_file->write(message);
			delete m_file;

			//Update timestamp file for the most recent timestamp, with a 
			//newline at the beginning
			ofstream time_filestream;	
			ThreadSafeFile* t_file = new ThreadSafeFile("timestamp.txt");
			std::stringstream sstm2;
			sstm2 << item->getTimestamp() << nl;
			std::string timestamp = sstm2.str();
			t_file->write(timestamp);
			delete t_file;

		}
		

		//Free fields
		delete item;

	}

	// Should never get here
	return NULL;
}