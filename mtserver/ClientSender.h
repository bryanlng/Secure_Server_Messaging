/*!
	Class that represents the "sender" of a client. This thread sends outgoing messages to the server.
	Once a message is sent,  the client's own log is updated.

	Function:
	1) First, create client_name.txt if they don't exist.
	2) Second, send an update message to the server to see if the client is behind on messages.
	3) Wait for user input. Once a user has typed in input, format it into a message, then send that message to the server 
*/

#ifndef __ClientSender_h__
#define __ClientSender_h__

#include "thread.h"
#include "tcpacceptor.h"
#include "wqueue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>

class ClientSender : public Thread {
	private:
		TCPStream* stream;		/*!<Object representing the data stream of a "connection"> */

	public:

		/**
		 *Constructor for a ClientSender.
		 *@param s the TCPStream that the WorkItem holds
		*/
		ClientSender(TCPStream* s);

		/**
		* starts up the ClientSender, and performs the function described in the class note.

		Function:
		1) First, create client_name.txt if they don't exist.
		2) Second, send an update message to the server to see if the client is behind on messages.
		3) Wait for user input. Once a user has typed in input, format it into a message, then send that message to the server 
		*/
		void* run();

		/**
		 *Appends a line of data (all the data from a MessageItem*) to the end of
		  the specified file. This file will most always be the server's master log.
		  This method is an exact replica of the one in MessageHandler.h
		 @param filename the file to write to. This file will most always be the server's master log.
		 @param line the line to be appended to the end of the file.
		*/
		void write(std::string filename, std::string line);

		/**
		* Returns the timestamp of the most recent message sent (as denoted by the latest message in the client's log).
		Reads the last line of client_log.txt, extracts the timestamp part out of it, then returns the timestamp in the form of a string
		This method is identical to readLatestTimestamp() in UpdateHandler.h, except we're reading out of the client's log, versus the server's master log.
		@return the latest timestamp of the most recent message sent
		*/
		std::string readLatestTimestamp();

		/**
		* Given 1 line from the client Log, this function extracts out the timestamp
		  This method is identical to readLatestTimestamp() in UpdateHandler.h, except we're reading out of the client's log, versus the server's master log.
		 @param the line from the Master log to extract the timestamp from
		 @return the extracted timestamp
		*/
		std::string extractTimestamp(std::string line);

		/**
		*Given a regular message, generates a formatted version with metatdata included that can be parsed by the server's master log.
		@param sender the author of this message (from client_name.txt)
		@param message the message to be sent
		@param delimiter the delimiter by which to delimit the message by. If the delimiter is "::Timestamp::", then we are creating an update message. If the delimiter is ":::::::", we are creating a regular message.
		@return 
		*/
		std::string formatMessage(std::string sender, std::string message, std::string delimiter);

		/**
		 * Destructor for ClientSender
		*/
		~ClientSender();

};
#endif