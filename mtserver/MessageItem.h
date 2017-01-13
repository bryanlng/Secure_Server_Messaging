/*
	Object to hold all the metadata of the message, as well as the message itself.
	
	After parsing incoming messages, the server stuffs all the data into a 
	MessageItem, then puts it onto a queue (wqueue) similar to the queue used to 
	manage WorkItems. 

	MessageItem objects will only exist on the server-side. Incoming messages from 
	clients as well as outgoing messages to clients will be in the form of strings.

	Format of message:
	timestamp <delimiter> date_formatted <delimiter> message <delimiter>
	thread_id is ignored, as that field is taken from the thread itself

	Delimiter:  ::&$*@^$^$(@(::

*/

class MessageItem
{
	private:
		string raw;					//raw form of the message, not split yet
		long timestamp;				//timestamp of when the message was sent, in milliseconds
		string date_formatted;		//Formatted date of when the message of sent
		string message;				//Actual message
		string thread_id;			//ID of thread (which held the connection) which sent the message
									//Used for broadcasting 

	public:
		MessageItem(std::string full, long time, std::string date, std::string mes, std::string tid) :
			raw(full), timestamp(time), date_formatted(date), message(mes), thread_id(tid) {}

		string getRawMessage() {
			return raw;
		}

		string getThreadID() {
			return thread_id;
		}

};