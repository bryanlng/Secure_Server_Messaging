#include "ClientSender.h"

class ClientReceiver : public Thread{
	private:
		TCPStream* stream;
	public:
		ClientReceiver(TCPStream* s);
		void* run();
		~ClientReceiver();
};
