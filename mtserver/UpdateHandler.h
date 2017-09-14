/*!
	Class that represents the Update Handler
	Reads from the master log, and sends messages to the user (represented by a ConnectionHandler)
	who is behind

	The server creates only 1 of these at the beginning.

	Function:
	1. Consumer threads remove and grab a MessageItem from the update queue
	   If there's no MessageItems in the message queue (like in the beginning),
	   then the UpdateHandler blocks.
	2. Then, compare the timestamp from the MessageItem to the server's most up to date timestamp, located in timestamp.txt.
		1) If the server's timestamp.txt isn't empty
			1) If the receieved timestamp < server's latest timestamp
				1) Find the ConnectionHandler* that represents the sender (who we're going to send the missed messages back to)
				2) Read all messages from the master log whose timestamp > received timestamp
				3) Send all messages back to the sender
		2) Else, do nothing
*/
#ifndef __UpdateHandler_h__
#define __UpdateHandler_h__

#include "ConnectionHandler.h"

class UpdateHandler : public Thread {
	private:
		vector<ConnectionHandler*>& connections;	/*!<Vector that holds all the current "clients", represented as ConnectionHandler*> */
		wqueue<MessageItem*>&		update_queue;	/*!<The update queue that the handler will pull MessageItem* objects from> */

	public:
		/**
		* Constructor for UpdateHandler. Takes in a vector that represents all the current clients, 
		  and a work queue of MessageItem* that represents the update queue, which the handler will pull MessageItem* objects from, and a name
		  @param connects the vector that represents all the current clients
		  @param queue the work queue of MessageItem* that represents the update queue
		  @param n the name of the UpdateHandler
		*/
		UpdateHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);

		/**
		* starts up the update handler, and performs the function described in the class note.
		  	Function:
			1. Consumer threads remove and grab a MessageItem from the update queue
			   If there's no MessageItems in the message queue (like in the beginning),
			   then the UpdateHandler blocks.
			2. Then, compare the timestamp from the MessageItem to the server's most up to date timestamp, which is the timestamp of the latest (last) line in master_log.txt
			1) If the server's master log isn't empty
					If the receieved timestamp < server's latest timestamp
						1) Find the ConnectionHandler* that represents the sender (who we're going to send the missed messages back to)
						2) Read all messages from the master log whose timestamp > received timestamp
						3) Send all messages back to the sender
				2) Else, do nothing	
		*/
		void* run();

		/**
		* Returns the timestamp of the most recent message sent (as denoted by the latest message in the server's master log). 
		 Reads the last line of master_log.txt, extracts the timestamp part out of it, then returns the timestamp in the form of a string
		 @return the latest timestamp of the most recent message sent
		*/
		std::string readLatestTimestamp();

		/**
		* Reads all messages from the server's message log whose timestamp is greater than the provided timestamp to a vector of messages
		 @param messages the vector that will contain all messages from the server's message log whose timestamp is greater than the provided timestamp
		 @param ts the provided timestamp
		*/
		void readMasterLog(std::vector<std::string>& messages, long long ts);

		/**
		* Given 1 line from the Master Log, this function extracts out the timestamp
		@param line from the Master log to extract the timestamp from
		@return the extracted timestamp
		*/
		std::string extractTimestamp(std::string line);
};

#endif