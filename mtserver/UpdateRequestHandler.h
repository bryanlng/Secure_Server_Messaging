#include "thread.h"
#include "ConnectionHandler.h"
class UpdateRequestHandler : public Thread
{
	private:
		list<ConnectionHandler*>& connections;		//list of all ConnectionHandler
		wqueue<MessageItem*>& update_queue;			//reference to the update queue, which manages requests to update

	public:
		UpdateRequestHandler(list<ConnectionHandler*>& connects, wqueue<UpdateItem*>& u_queue, std::string n);
		void* run();
};