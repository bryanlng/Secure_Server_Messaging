#include "UpdateRequestHandler.h"
UpdateRequestHandler::UpdateRequestHandler(list<ConnectionHandler*>& connects, 
	wqueue<MessageItem*>& u_queue, std::string n)
	: connections(connects), update_queue(u_queue)
{
	set_name(n);
}
void* UpdateRequestHandler::run() {

}