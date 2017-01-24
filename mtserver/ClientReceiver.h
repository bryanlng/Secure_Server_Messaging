#include "thread.h"
#include "tcpacceptor.h"
#include "ThreadSafeFile.h"
#include <iostream>
#include <fstream>

class ClientReceiver : public Thread{
	private:
		TCPStream* stream;
	public:
		ClientReceiver(TCPStream* s);
		void* run();
		~ClientReceiver();
};
