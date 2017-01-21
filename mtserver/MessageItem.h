/*
	Object to hold all the metadata of the message, as well as the message itself.
	
	After parsing incoming messages, the server stuffs all the data into a 
	MessageItem, then puts it onto a queue (wqueue) similar to the queue used to 
	manage WorkItems. 

	MessageItem objects will only exist on the server-side. Incoming messages from 
	clients as well as outgoing messages to clients will be in the form of strings.

	Format of message:
	Case 1: Regular message
		-timestamp <delimiter> date_formatted <delimiter> message <delimiter>
		-thread_id is ignored, as that field is taken from the thread itself
		-Delimiter:  ::&$*@^$^$(@(::
		-time_of_last_received = -1
		-Constructor 1 takes care of this

	Case 2: Timestamp message
		-timestamp <special delimiter>
		-Delimiter:	 ::Timestamp!!!!!::
		-time_of_last_received != -1
		-timestamp = -1
		-All string fields are ""
		-Constructor 2 takes care of this

	Constructor 3 is used in the process of sending messages from the master log
	to the client. 

	Timestamp of last message received:
		-special message sent by client
		-If it's just a regular message, time_of_last_received = -1
		-Else, time_of_last_received != -1

*/

class MessageItem
{
	private:
		string raw;					//raw form of the message, not split yet
		long time_of_last_received; //Timestamp of the last message the CLIENT received
		long timestamp;				//timestamp of when the message was sent, in milliseconds
		string date_formatted;		//Formatted date of when the message of sent
		string message;				//Actual message
		string thread_id;			//ID of thread (which held the connection) which sent the message
									//Used for broadcasting 

	public:
		MessageItem(std::string full, long last_received, long time, std::string date, std::string mes, std::string tid) :
			raw(full), time_of_last_received(last_received) ,timestamp(time), 
			date_formatted(date), message(mes), thread_id(tid) {}

		MessageItem(std::string full) : raw(full), time_of_last_received(-1){}

		MessageItem(long last_received) : time_of_last_received(last_received) {}

		string getRawMessage() {
			return raw;
		}

		string getThreadID() {
			return thread_id;
		}

		bool isUpdateRequest() {
			return !(time_of_last_received == -1);
		}

		long getTimestamp() {
			return timestamp;
		}

		long getTimeOfLastReceived() {
			return time_of_last_received;
		}

};