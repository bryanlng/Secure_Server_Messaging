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
		long time_of_last_received = -1;		//used to check whether we need to pull from log
		long timestamp = -1;
		string date_formatted;
		string message;

		//Upon receiving a message, parse it, then put contents into a MessageItem
		//Format of message from client:
		/*
			Case 1. Timestamp/update message
			timestamp <special delimiter>

			Case 2. Regular message
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>

		*/
		MessageItem* message_item;
		while ((len = stream->receive(input, MAX_MESSAGE_SIZE - 1) > 0)) {
			std::cout << "Raw message received from client: " << input << std::endl;
			string raw(input);
			raw_message = raw;

			//Fields for parsing the string
			int delimiter_pos = 0;
			int which_one = 0;
			string current_item;

			//First check if the message is the "timestamp" message
			string delimiter = "??";	//"::Timestamp!!!!!::";
			if ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
				current_item = raw.substr(0, delimiter_pos);
				std::cout << "Timestamp message, Item found: " << current_item << std::endl;
				time_of_last_received = atol(current_item.c_str());
			}

			//Else, it's got to be just a regular message
			else {
				delimiter = "::";	//"::&$*@^$^$(@(::";

				//Parsing the string. String is "eaten" along the way
				while ((delimiter_pos = raw.find(delimiter)) != std::string::npos) {
					current_item = raw.substr(0, delimiter_pos);
					std::cout << "Regular message, Item found: " << current_item << std::endl;
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
					}

					//update fields
					int next_index = delimiter_pos + delimiter.length();
					raw = raw.substr(next_index, raw.size());
					++which_one;
				}

			}

			std::cout << "Timestamp: " << timestamp << std::endl;
			std::cout << "time_of_last_received: " << time_of_last_received << std::endl;

			//Create a new message item
			message_item = new MessageItem(raw_message, time_of_last_received, timestamp, date_formatted, message, thread_name());
			
			//Add the new message item to the appropriate message queue
			if (message_item->isUpdateRequest()) {
				update_queue.add(message_item);
			}
			else {
				m_queue.add(message_item);
			}
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
		sstm << message_item->getTimeOfLastReceived() << "??";	//special delimiter added so client knows
		message = sstm.str();

		std::cout << "send_message(): Updated timestamp being sent: " << message << std::endl;
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
	
	printf("message being sent, in char* form: %s\n", buffer);

	//Send message, then free temp buffer
	stream->send(buffer, message.size()+1);	//+1 to include the null char at the end
	delete[] buffer;	

}

/*
	Destructor
*/
ConnectionHandler::~ConnectionHandler() {
	delete stream;
}