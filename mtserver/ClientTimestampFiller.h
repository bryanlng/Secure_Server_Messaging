#include "ClientReceiver.h"

class ClientTimestampFiller : public Thread {
	private:
		wqueue<long>& m_queue;

	public:
		ClientTimestampFiller(wqueue<long>& queue, std::string n);
		void* run();
};	
