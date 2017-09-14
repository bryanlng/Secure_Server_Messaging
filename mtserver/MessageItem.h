/*!
	Object to hold all the metadata of the message, as well as the message itself.
	
	After parsing incoming messages, the server stuffs all the data into a 
	MessageItem, then puts it onto a queue (wqueue) similar to the queue used to 
	manage WorkItems. 

	MessageItem objects will only exist on the server-side. Incoming messages from 
	clients as well as outgoing messages to clients will be in the form of strings.

	Format of message:
	Case 1: Regular message
		-timestamp delimiter date_formatted delimiter message delimiter sender delimiter
		-thread_id is ignored, as that field is taken from the thread itself
		-Delimiter:  ":::::::"
		-time_of_last_received = -1
		-Constructor 1 takes care of this

	Case 2: Timestamp message
		-timestamp special timestamp delimiter
		-Delimiter:	 "::Timestamp::"
		-time_of_last_received != -1
		-timestamp = -1
		-All string fields are ""
		-Constructor 2 takes care of this

	Constructor 3 is used in the process of sending messages from the master log
	to the client. 

	Timestamp of last message received:
		-special message sent by client
		-If it's just a regular message, time_of_last_received = -1
		-Else, time_of_last_received != -1
*/
#ifndef __MessageItem_h__
#define __MessageItem_h__

class MessageItem
{
	private:
		string		raw;					/*!<raw form of the message, not split yet> */
		long long	time_of_last_received;	/*!<Timestamp of the last message the CLIENT received> */
		long long	timestamp;				/*!<timestamp of when the message was sent, in milliseconds> */
		string		date_formatted;			/*!<Formatted date of when the message of sent> */
		string		message;				/*!<Actual message> */
		string		sender;					/*!<Name of sender> */
		string		thread_id;				/*!<ID of thread (which held the connection) which sent the message> */

	public:

		/**
		 *Constructor for a MessageItem. 
		 @param full the raw form of the message, not split yet
		 @param last_received Timestamp of the last message the CLIENT received
		 @param time timestamp of when the message was sent, in milliseconds
		 @param date Formatted date of when the message of sent
		 @param mes Actual message
		 @param n Name of sender
		 @param tid ID of thread (which held the connection) which sent the message
		*/
		MessageItem(std::string full, long long last_received, long long time, std::string date, std::string mes, std::string n, std::string tid) :
			raw(full), time_of_last_received(last_received) ,timestamp(time), 
			date_formatted(date), message(mes), sender(n), thread_id(tid) {}

		/**
		 *Alternative constructor for a MessageItem, that creates a MessageItem given only the raw string.
		 @param full the raw form of the message, not split yet
		*/
		MessageItem(std::string full) : raw(full), time_of_last_received(-1){}

		/**
		 *Alternative constructor for a MessageItem, that creates a MessageItem given only the time of last received
		 @param last_received the timestamp of the last message the CLIENT received
		*/
		MessageItem(long long last_received) : time_of_last_received(last_received) {}

		/**
		 *Gets the raw message from the MessageItem
		 @return the raw message from the MessageItem
		*/
		string getRawMessage() {
			return raw;
		}

		/**
		*Gets the thread id from the MessageItem
		@return the thread id from the MessageItem
		*/
		string getThreadID() {
			return thread_id;
		}
		
		/**
		*Gets whether the MessageItem* represents an update request or not.
		 A MessageItem* is an update request if time_of_last_received != -1. time_of_last_received will be instead be the timestamp of the last message
		 that client received.
		@return whether the MessageItem* represents an update request or not.
		*/
		bool isUpdateRequest() {
			return !(time_of_last_received == -1);
		}

		/**
		 *Gets the timestamp of the last message that the client received.
		  A MessageItem* is an update request if time_of_last_received != -1. time_of_last_received will be instead be the timestamp of the last message
		 @return the timestamp of the last message that the client received.
		*/
		long getTimeOfLastReceived() {
			return time_of_last_received;
		}

};

#endif