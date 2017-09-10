#ifndef __UpdateHandler_h__
#define __UpdateHandler_h__

#include "ConnectionHandler.h"
/*
	Class that represents the Update Handler
	Reads from the master log, and sends messages to the user (represented by a ConnectionHandler)
	who is behind

	The server creates only 1 of these at the beginning.

	Function:
	1. Consumer threads remove and grab a MessageItem from the update queue
	   If there's no MessageItems in the message queue (like in the beginning),
	   then the UpdateHandler blocks.
	2. Then, compare the timestamp from the MessageItem to the server's most up to date timestamp, located in timestamp.txt
		1) If the server's timestamp.txt isn't empty
			If the receieved timestamp < server's latest timestamp
				1) Find the ConnectionHandler* that represents the sender (who we're going to send the missed messages back to)
				2) Read all messages from the master log whose timestamp > received timestamp
				3) Send all messages back to the sender
		2) Else, do nothing

*/
class UpdateHandler : public Thread {
	private:
		vector<ConnectionHandler*>& connections;
		wqueue<MessageItem*>& m_queue;

	public:
		UpdateHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);
		void* run();
		std::string readTimestamp();
		std::string extractTimestamp(std::string line);
		void readMasterLog(std::vector<std::string>& messages, long long ts);
};

#endif