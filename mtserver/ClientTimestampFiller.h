#include "ClientReceiver.h"
/*
	Class that fills timestamp.txt, the file that holds the latest timestamp for the server
	I decided to have the timestamp in a separate file, as to reduce lookup time for the server's timestamp
	during the update process. Otherwise, I would have to parse through 
	Takes a message and broadcasts it to all the Consumer Threads that:
	1) Currently have a connection (TCPStream* object)
	2) Isn't the sender of the message

	The server creates only 1 of these at the beginning.
*/
class ClientTimestampFiller : public Thread {
	private:
		wqueue<long long>& m_queue;

	public:
		ClientTimestampFiller(wqueue<long long>& queue, std::string n);
		void* run();
};	
