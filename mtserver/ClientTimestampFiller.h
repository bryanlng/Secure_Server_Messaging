#include "ClientReceiver.h"

class ClientTimestampFiller : public Thread {
	private:
		wqueue<long long>& m_queue;

	public:
		ClientTimestampFiller(wqueue<long long>& queue, std::string n);
		void* run();
};	
