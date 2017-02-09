#include "ClientSender.h"

/*
	Thread that encapsulates the functionality to receive messages
	Once a message is received, the apppropriate fields are extracted,
	the message is displayed, and the timestamp file is updated.

http://stackoverflow.com/questions/10486116/what-does-this-gcc-error-relocation-truncated-to-fit-mean
http://stackoverflow.com/questions/39931468/what-does-mean-when-used-in-a-g-argument
*/
class ClientReceiver : public Thread{
	private:
		TCPStream* stream;
		wqueue<long>& m_queue;

	public:
		ClientReceiver(TCPStream* s, wqueue<long long>& queue);
		void* run();
		~ClientReceiver();
};
