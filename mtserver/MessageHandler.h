#ifndef __MessageHandler_h__
#define __MessageHandler_h__

#include "ConnectionHandler.h"

/*
	Class that represents the Message Handler
	Takes a message and broadcasts it to all the Consumer Threads that:
		1) Currently have a connection (TCPStream* object)
		2) Isn't the sender of the message

	The server creates only 1 of these at the beginning.
*/
class MessageHandler : public Thread {
	private:
		vector<ConnectionHandler*>& connections;
		wqueue<MessageItem*>& m_queue;

	public:
		MessageHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);
		void* run();
		void write(std::string filename, std::string item);
};

#endif
