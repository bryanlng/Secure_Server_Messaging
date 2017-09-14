/*!
	Class that represents the "receiver" of a client. This thread receives incoming messages from the server.
	Once a message is received, the apppropriate fields are extracted, the message is displayed, and the client's own log is updated.

	Function:
	1) Extract the timestamp, date, and message.
	2) Then, display the message
	3) Afterwards, update client_log.txt with the raw message.
	-Format:
	timestamp delimiter date_formatted delimiter message delimiter
	Ex: 1485288373:Tue Jan 24 13:06:13 2017:df:

*/
#ifndef __ClientReceiver_h__
#define __ClientReceiver_h__

#include "ClientSender.h"

class ClientReceiver : public Thread{
	private:
		TCPStream* stream;	/*!<Object representing the data stream of a "connection"> */

	public:
		/**
		 *Constructor for a ClientReceiver.
		 *@param s the TCPStream that the WorkItem holds
		*/
		ClientReceiver(TCPStream* s);

		/**
		 * starts up the ClientReceiver, and performs the function described in the class note.
		   
		    Function:
			1) Extract the timestamp, date, and message.
			2) Then, display the message
			3) Afterwards, update client_log.txt with the raw message.
			-Format:
			timestamp <delimiter> date_formatted <delimiter> message <delimiter>
			Ex: 1485288373:Tue Jan 24 13:06:13 2017:df:		  
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
		 *Destructor for ClientReceiver
		*/
		~ClientReceiver();
};
#endif