#include "MessageItem.h"
class UpdateItem
{
	private:
		MessageItem message_item;		//Represents the message 
		long timestamp;					//timestamp of the last message the CLIENT received

	public:
		UpdateItem(MessageItem mess, long time) : message_item(mess), timestamp(time){}

		MessageItem getMessageItem() {
			return message_item;
		}

		long getTimestamp() {
			return timestamp;
		}

};