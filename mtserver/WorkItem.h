/*
	A WorkItem consists of a TCPStream object, denoted by the
	TCPStream* m_stream

	WorkItems encapsulate the incoming connection, and are put
	onto a queue (wqueue), which distributes connections to the 
	given Threads.
*/

#include "tcpacceptor.h"
class WorkItem
{
private:
	TCPStream* m_stream;

public:
	WorkItem(TCPStream* stream) : m_stream(stream) {}
	~WorkItem() { delete m_stream; }

	TCPStream* getStream() { return m_stream; }
};
