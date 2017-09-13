/*!
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
*/

#ifndef __ConnectionHandler_h__
#define __ConnectionHandler_h__

#include "WorkItem.h"
#include "wqueue.h"
#include "thread.h"
#include "MessageItem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include <chrono>
#define NEW_LINE_VAL 10

class ConnectionHandler : public Thread
{
	private:
		wqueue<WorkItem*>&		w_queue;		/*!<reference to the work queue, which holds incoming transactions> */
		wqueue<MessageItem*>&	m_queue;		/*!<reference to the message queue, which holds messages pending to be processed> */
		wqueue<MessageItem*>&	update_queue;	/*!<reference to the update queue, which holds pending update requests (in the form of MessageItem*)> */
		TCPStream*				stream;			/*!<TCPStream that the ConnectionHandler is managing> */				
		bool					connected;		/*!<Does the Thread currently have a connection?> */	

	public:
		/**
		 *Constructor for a ConnectionHandler.
		 @param queue reference to the work queue, which holds incoming transactions
		 @param message_queue reference to the message queue, which holds messages pending to be processed
		 @param update_queue reference to the update queue, which holds pending update requests (in the form of MessageItem*)
		 @param n the name of the thread
		*/
		ConnectionHandler(wqueue<WorkItem*>& queue, wqueue<MessageItem*>& message_queue, wqueue<MessageItem*>& update_queue, std::string n);

		/**
		* starts up the ConnectionHandler, and performs the function described in the class note.
		  
		  Function:
		  1. Consumer threads remove and grab a WorkItem from the work queue, then get
		  their TCPStream --> then get their message, which is a string demarcated
		  by some delimiter.
		  2. After parsing the contents of the string into fields, we put all the data
		  into a MessageItem, then put it onto the message queue.

		  If there's no WorkItems in the work queue (like in the beginning), the
		  ConnectionHandler blocks.
		*/
		void* run();

		/**
		 *Gets the TCPStream* object of the ConnectionHandler*
		 @return the TCPStream* object of the ConnectionHandler*
		*/
		TCPStream* getStream();

		/**
		 *Sets the TCPStream* object of the ConnectionHandler*
		 @param s the TCPStream* object of the ConnectionHandler*
		*/
		void setStream(TCPStream* s);

		/**
		*Determines if the Thread currently has a connection
		@return whether or not the Thread currently has a connection
		*/
		bool hasAConnection();

		/**
		 *Sends a message back to the client connection, which is defined by the TCPStream* 
		that we got from the WorkItem that we pulled off the work queue in run()
		 @param message_item the data to send back to the client
		*/
		void send_message(MessageItem* message_item);

		/**
		 *Destructor for the ConnectionHandler
		*/
		~ConnectionHandler();
};

#endif