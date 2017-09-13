/*!
	NOTICE:
	This file is from the original implementation by Vic
	Hargrave.
	
	A WorkItem consists of a TCPStream object, denoted by the
	TCPStream* m_stream

	WorkItems encapsulate the incoming connection, and are put
	onto a queue (wqueue), which distributes connections to the 
	given Threads.
*/

#ifndef __WorkItem_h__
#define __WorkItem_h__

#include "tcpacceptor.h"
class WorkItem
{
private:
	TCPStream* m_stream;	/*!<Object representing the data stream of a "connection"> */

public:
	/**
	 *Constructor for a WorkItem. A WorkItem consists of a TCPStream object, denoted by the
	 TCPStream* m_stream. WorkItems encapsulate the incoming connection, and are put
	 onto a queue (wqueue), which distributes connections to the 
	 given Threads.
	 *@param stream the TCPStream that the WorkItem holds
	 */
	WorkItem(TCPStream* stream) : m_stream(stream) {}

   /**
	*Destructor for a WorkItem. Simply deletes the TCPStream object it holds
	*/
	~WorkItem() { delete m_stream; }

	/**
	 *Gets the TCPStream object contained inside the WorkItem.
	 *@return the TCPStream object contained inside the WorkItem.
	*/
	TCPStream* getStream() { return m_stream; }
};

#endif