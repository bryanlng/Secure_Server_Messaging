#ifndef __UpdateHandler_h__
#define __UpdateHandler_h__

#include "ConnectionHandler.h"
/*
	Class that represents the Message Handler
	Takes a message and broadcasts it to all the Consumer Threads that:
		1) Currently have a connection (TCPStream* object)
		2) Isn't the sender of the message

	The server creates only 1 of these at the beginning.

	Function:
	1. Consumer threads remove and grab a MessageItem from the message queue
	   If there's no MessageItems in the message queue (like in the beginning),
	   then the MessageHandler blocks.
	2. Then, "broadcast" the message to every Consumer thread that:
		  1) Currently have a connection (TCPStream* object)
		  2) Isn't the sender of the message
	   by calling each Consumer Thread's send_message()

*/
class UpdateHandler : public Thread
{
	private:
		vector<ConnectionHandler*>& connections;
		wqueue<MessageItem*>& m_queue;

	public:
		UpdateHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);
		void* run();
		std::string readTimestampFile();
		void readMasterLog(std::vector<std::string>& messages, long long ts);
		void write(std::string filename, std::string item);
};

#endif