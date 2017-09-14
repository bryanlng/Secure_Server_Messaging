/*!
Class that represents the Message Handler

Function:
The Message Handler takes in a message and broadcasts it to all the Consumer Threads that:
	1) Currently have a connection (TCPStream* object)
	2) Isn't the sender of the message

The server creates only 1 of these at the beginning.
*/
#ifndef __MessageHandler_h__
#define __MessageHandler_h__

#include "ConnectionHandler.h"


class MessageHandler : public Thread {
	private:
		vector<ConnectionHandler*>& connections;		/*!<Vector that holds all the current "clients", represented as ConnectionHandler*> */
		wqueue<MessageItem*>&		m_queue;			/*!<reference to the message queue, which holds messages pending to be processed> */

	public:
		/**
		 *Constructor for a MessageHandler.
		 @param connects that holds all the current "clients", represented as ConnectionHandler*
		 @param queue reference to the message queue, which holds messages pending to be processed
		 @param n the name of the thread
		*/
		MessageHandler(vector<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);

		/**
		* starts up the MessageHandler, and performs the function described in the class note.

		Function:
		The Message Handler takes in a message and broadcasts it to all the Consumer Threads that:
			1) Currently have a connection (TCPStream* object)
			2) Isn't the sender of the message
		*/
		void* run();

		/**
		 *Appends a line of data (all the data from a MessageItem*) to the end of 
		  the specified file. This file will most always be the server's master log.
		 @param filename the file to write to. This file will most always be the server's master log.
		 @param line the line to be appended to the end of the file.
		*/
		void write(std::string filename, std::string line);
};

#endif
