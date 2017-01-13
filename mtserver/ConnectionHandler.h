#include "WorkItem.h"
#include "wqueue.h"
#include "thread.h"
#include "MessageItem.h"
#include <iostream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <list>
#include <stdio.h>

class ConnectionHandler : public Thread
{
private:
	list<ConnectionHandler*>& connections;		//list of all ConnectionHandler
	wqueue<WorkItem*>& w_queue;					//reference to the work queue that manages all the ConnectionHandler
	wqueue<MessageItem*>& m_queue;				//reference to the message queue that manages all the messages
	TCPStream* stream;							//TCPStream that the ConnectionHandler is managing
	bool connected;								//Does the Thread currently have a connection?

public:
	ConnectionHandler(list<ConnectionHandler*>& connects, wqueue<WorkItem*>& queue, wqueue<MessageItem*>& message_queue, std::string n);
	void* run();
	TCPStream* getStream();
	void setStream(TCPStream* s);
	bool hasAConnection();
	void send_message(MessageItem* message_item);
};