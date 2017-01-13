#include "ConnectionHandler.h"
#define MAX_MESSAGE_SIZE 25600


ConnectionHandler::ConnectionHandler(list<ConnectionHandler*>& connects, wqueue<WorkItem*>& queue, wqueue<MessageItem*>& message_queue, std::string n)
	: connections(connects), w_queue(queue), m_queue(message_queue)
{
	set_name(n);
	connected = false;
}

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
		string date_formatted;
		string message;

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
				case 3:
					break;
				default:
					std::cout << "Should not get here" << std::endl;
					break;
				}

				//update fields
				int next_index = delimiter_pos + delimiter.length();
				raw = raw.substr(next_index, raw.size());
				++which_one;
			}

			//Create a new message item and add it to the message queue
			message_item = new MessageItem(raw_message, timestamp, date_formatted, message, thread_name());
			m_queue.add(message_item);
		}
		free(input);
		delete item;
		connected = false;

	}

	// Should never get here
	return NULL;
}

TCPStream* ConnectionHandler::getStream() {
	return stream;
}

void ConnectionHandler::setStream(TCPStream* s) {
	stream = s;
}

bool ConnectionHandler::hasAConnection() {
	return connected;
}

void ConnectionHandler::send_message(MessageItem* message_item) {
	TCPStream* stream = getStream();
	string raw_message = message_item->getRawMessage();
	std::cout << "send_message(): Raw message being sent: " << raw_message << std::endl;

	//Convert message from string --> c-style string, since send() only accepts a char*
	char* c_string = new char[raw_message.size() + 1];
	std::copy(raw_message.begin(), raw_message.end(), c_string);
	c_string[raw_message.size()] = '\0';

	printf("c_string: %s\n", c_string);
	//Send message, then free temp buffer
	stream->send(const_cast<const char*>(c_string), raw_message.size());
	//delete(c_string);
}
