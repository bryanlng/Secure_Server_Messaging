#include "thread.h"
#include "tcpacceptor.h"

/*
	http://stackoverflow.com/questions/10486116/what-does-this-gcc-error-relocation-truncated-to-fit-mean
	http://stackoverflow.com/questions/39931468/what-does-mean-when-used-in-a-g-argument
*/
class ClientSender : public Thread {
	private:
		TCPStream* stream;
	public:
		ClientSender(TCPStream* s);
		void* run();
		TCPStream* getStream();
		~ClientSender();

};