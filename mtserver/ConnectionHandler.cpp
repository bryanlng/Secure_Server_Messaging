#include "ConnectionHandler.h"
#define MAX_MESSAGE_SIZE 25600


ConnectionHandler::ConnectionHandler(wqueue<WorkItem*>& queue, wqueue<MessageItem*>& message_queue, wqueue<MessageItem*>& u_queue, std::string n)
	: w_queue(queue), m_queue(message_queue), update_queue(u_queue)
{
	set_name(n);
	connected = false;
}

/*
	Function:
	1. Consumer threads remove and grab a WorkItem from the work queue, then get 
	   their TCPStream --> then get their message, which is a string demarcated 
	   by some delimiter. 
	2. After parsing the contents of the string into fields, we put all the data 
	   into a MessageItem, then put it onto the message queue.

	If there's no WorkItems in the work queue (like in the beginning), the 
	ConnectionHandler blocks.
*/
void* ConnectionHandler::run() {
	// Remove 1 item at a time and process it. Blocks if no items are 
	// available to process.
	for (int i = 0;; i++) {
		std::cout << thread_name() << ", loop " << i << " - waiting for item..." << std::endl;
		WorkItem* item = w_queue.remove();
		std::cout << thread_name() << ", loop " << i << " - got one item..." << std::endl;
		TCPStream* stream = item->getStream();
		setStream(stream);
		connected = true;

		// 1. Parse contents into their fields ==> then put into a MessageItem object
		// Delimiter will be: ::&$*@^$^$(@(::
		// Delimiter will not be seen by the user

		//Fields for receiving the message
		char* input;
		input = (char*)malloc(sizeof(char) * MAX_MESSAGE_SIZE);
		int len;

		//Fields to put inside the Message item
		string raw_message;
		long timestamp;
		//long time_of_last_received;		//used to check whether we need to pull from log
		string date_formatted;
		string message;

		//Upon receiving a message, parse it, then put contents into 
		//a MessageItem
		//Format of message from client:
		/*
			timestamp <delimiter> timestamp of the last message received<delimiter> 
			date_formatted <delimiter> message <delimiter>
		*/
		MessageItem* message_item;
		while ((len = stream->receive(input, MAX_MESSAGE_SIZE - 1) > 0)) {
			std::cout << "Raw message received from client: " << input << std::endl;
			string raw(input);
			raw_message = raw;

			//Fields for parsing the string
			string delimiter = ":";	//"::&$*@^$^$(@(::";
			int delimiter_pos = 0;
			int which_one = 0;
			string current_item;

			//Parsing the string. String is "eaten" along the way
			while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
				current_item = raw.substr(0, delimiter_pos);
				std::cout << "Item found: " << current_item << std::endl;
				switch (which_one) {
					case 0:		//timestamp
						timestamp = atol(current_item.c_str());
						break;
					case 1:		//formatted date
						date_formatted = current_item;
						break;
					case 2:		//actual message
						message = current_item;
						break;
					default:
						std::cout << "Field is not used" << std::endl;
						break;
					//case 0:		//timestamp
					//	timestamp = atol(current_item.c_str());
					//	break;
					//case 1:		//timestamp of the last message received
					//	time_of_last_received = atol(current_item.c_str());
					//	break;
					//case 2:		//formatted date
					//	date_formatted = current_item;
					//	break;
					//case 3:		//actual message
					//	message = current_item;
					//	break;
					//default:
					//	std::cout << "Field is not used" << std::endl;
					//	break;
				}

				//update fields
				int next_index = delimiter_pos + delimiter.length();
				raw = raw.substr(next_index, raw.size());
				++which_one;
			}

			//Create a new message item
			message_item = new MessageItem(raw_message, timestamp, date_formatted, message, thread_name());
			
			//First check if connection is behind. AKA, do we need to download messages
			//from the master log?

			//Add the new message item to the message queue
			m_queue.add(message_item);
		}

		//Free and update fields
		free(input);
		delete item;
		connected = false;

	}

	// Should never get here
	return NULL;
}

/*
	Gets the TCPStream* that represents the connection
*/
TCPStream* ConnectionHandler::getStream() {
	return stream;
}

/*
	Sets the TCPStream* that represents the connection
*/
void ConnectionHandler::setStream(TCPStream* s) {
	stream = s;
}

/*
	Returns whether or not the ConnectionHandler has a connection
	In other words, have we successfully pulled a WorkItem from the work queue?
*/
bool ConnectionHandler::hasAConnection() {
	return connected;
}

/*
	Sends a message back to the client connection, which is defined
	by the TCPStream* that we got from the WorkItem that we pulled
	off the work queue in run()
*/
void ConnectionHandler::send_message(MessageItem* message_item) {
	TCPStream* stream = getStream();
	string raw_message = message_item->getRawMessage();
	std::cout << "send_message(): Raw message being sent: " << raw_message << std::endl;

	//Convert message from string --> c-style string, since send() only accepts a char*
	char* c_string = new char[raw_message.size() + 1];
	std::copy(raw_message.begin(), raw_message.end(), c_string);
	c_string[raw_message.size()] = '\0';

	//Send message, then free temp buffer
	printf("c_string: %s\n", c_string);
	stream->send(const_cast<const char*>(c_string), raw_message.size());
	//delete(c_string);

	//Update the most recent timestamp file to be the timestamp of the message,
	//as this message will be the most recent message sent
	

}
