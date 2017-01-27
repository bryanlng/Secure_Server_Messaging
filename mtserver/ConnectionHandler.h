#include "WorkItem.h"
#include "wqueue.h"
#include "thread.h"
#include "MessageItem.h"
#include "ThreadSafeFile.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NEW_LINE_VAL 10
/*
	Class that represents a "Consumer Thread"
	The server creates a specified number of Consumer Threads at the beginning.

	Function:
	1. Consumer threads remove and grab a WorkItem from the work queue, then get 
	   their TCPStream --> then get their message, which is a string demarcated 
	   by some delimiter. 
	2. After parsing the contents of the string into fields, we put all the data 
	   into a MessageItem, then put it onto the message queue.

	If there's no WorkItems in the work queue (like in the beginning), the 
	ConnectionHandler blocks.

	http://stackoverflow.com/questions/6199729/how-to-solve-munmap-chunk-invalid-pointer-error-in-c
	http://stackoverflow.com/questions/3233987/deleting-a-reference
*/
class ConnectionHandler : public Thread
{
	private:
		wqueue<WorkItem*>& w_queue;					//reference to the work queue that manages all the ConnectionHandler
		wqueue<MessageItem*>& m_queue;				//reference to the message queue that manages all the messages
		wqueue<MessageItem*>& update_queue;			//reference to the update queue, which manages requests to update
		TCPStream* stream;							//TCPStream that the ConnectionHandler is managing
		bool connected;								//Does the Thread currently have a connection?

	public:
		ConnectionHandler(wqueue<WorkItem*>& queue, wqueue<MessageItem*>& message_queue, wqueue<MessageItem*>& update_queue, std::string n);
		void* run();
		TCPStream* getStream();
		void setStream(TCPStream* s);
		bool hasAConnection();
		void send_message(MessageItem* message_item);
		~ConnectionHandler();
};