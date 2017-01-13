#include "ConnectionHandler.h"

class MessageHandler : public Thread
{
	private:
		list<ConnectionHandler*>& connections;
		wqueue<MessageItem*>& m_queue;

	public:
		MessageHandler(list<ConnectionHandler*>& connects, wqueue<MessageItem*>& queue, std::string n);
		void* run();
};
