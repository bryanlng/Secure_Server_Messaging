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
	// Remove 1 item at a time and process it. Blocks if no items are available to process.
	for (int i = 0;; i++) {
		WorkItem* item = w_queue.remove();

		//Grab the TCPStream object from the WorkItem, and set it to the Stream field, so that send_message() can be called from a different Context later.
		TCPStream* stream = item->getStream();
		setStream(stream);
		connected = true;

		// 1. Upon receiving a message, parse it, then put contents into a MessageItem
		// Delimiter for a regular message will be ":::::::", delimiter for a timestamp message will be "::Timestamp::"
		//Format of message from client:
		/*
			Case 1. Timestamp/update message
			timestamp::Timestamp::

			Case 2. Regular message
			timestamp:::::::date_formatted:::::::message:::::::name:::::::
		*/

		MessageItem* message_item;
		char* input;
		input = (char*)malloc(sizeof(char) * MAX_MESSAGE_SIZE);
		int len;
		while ((len = stream->receive(input, MAX_MESSAGE_SIZE - 1) > 0)) {
			std::cout << "Raw message received from client: " << input << std::endl;
			string raw(input);
			
			//Fields to put inside the Message item
			string raw_message = raw;
			long long time_of_last_received = -1;		//used to check whether we need to pull from log
			long long timestamp = -1;
			string date_formatted;
			string message;
			string sender;		

			//Fields for parsing the string
			int delimiter_pos = 0;
			int which_one = 0;
			string current_item;

			//First check if the message is an update request
			string delimiter = "::Timestamp::";
			if ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
				current_item = raw.substr(0, delimiter_pos);
				std::cout << "Timestamp message, Item found: " << current_item << std::endl;
				time_of_last_received = atoll(current_item.c_str());
			}

			//Else, it's got to be just a regular message
			else {
				delimiter = ":::::::";

				//Parsing the string. String is "eaten" along the way
				while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
					current_item = raw.substr(0, delimiter_pos);
					std::cout << "Regular message, Item found: " << current_item << std::endl;
					switch (which_one) {
						case 0:		//timestamp
							timestamp = atoll(current_item.c_str());
							break;
						case 1:		//formatted date
							date_formatted = current_item;
							break;
						case 2:		//actual message
							message = current_item;
							break;
						case 3:		//name of sender
							sender = current_item;
							break;
						default:
							std::cout << "Field is not used" << std::endl;
							break;
					}

					//update fields for next iteration
					int next_index = delimiter_pos + delimiter.length();
					raw = raw.substr(next_index, raw.size());
					++which_one;
				}

			}

			//Create a new message item
			message_item = new MessageItem(raw_message, time_of_last_received, timestamp, date_formatted, message, sender, thread_name());
			
			//Add the new message item to the appropriate message queue
			if (message_item->isUpdateRequest()) {
				update_queue.add(message_item);
			}
			else {
				m_queue.add(message_item);
			}

			//"Erase" the char array, so our next message doesn't come in with bits of the previous message
			memset(input, 0, MAX_MESSAGE_SIZE);

		}

		//Free and update fields
		free(input);
		connected = false;
		delete item;

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

	Case 1: Updated timestamp message
	Convert the message from long --> string --> char*, then send it
	using TCPStream's send()

	Case 2: Regular message
	Convert the message from string --> char*, then send it
	using TCPStream's send()
*/
void ConnectionHandler::send_message(MessageItem* message_item) {
	TCPStream* stream = getStream();
	std::string message;

	//Case 1: Updated timestamp message
	if (message_item->isUpdateRequest()) {
		std::stringstream sstm;
		sstm << message_item->getTimeOfLastReceived() << "::Timestamp::";	//special delimiter added so client knows
		message = sstm.str();
	}

	//Case 2: Regular message
	else {
		message = message_item->getRawMessage();
		std::cout << "send_message(): Regular message being sent: " << message << std::endl;
	}

	//Convert message from string --> c-style string, since send() only accepts a char*
	char * buffer = new char[message.length() + 1];
	const char* c_string = message.c_str();
	std::strcpy(buffer, c_string);	

	//Send message, then free temp buffer
	stream->send(buffer, message.size()+1);	//+1 to include the null char at the end
	free(buffer);

}

/*
	Destructor
*/
ConnectionHandler::~ConnectionHandler() {
	delete stream;
}