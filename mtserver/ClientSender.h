#include "thread.h"
#include "tcpacceptor.h"
#include "ThreadSafeFile.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <string>
#include <vector>
#include <algorithm>
#include <climits>


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
		std::string formatMessage(std::string message, std::string delimiter);
		~ClientSender();

};