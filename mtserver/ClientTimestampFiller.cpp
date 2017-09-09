#include "ClientTimestampFiller.h"

ClientTimestampFiller::ClientTimestampFiller(wqueue<long long>& queue, std::string n) : m_queue(queue) 
{
	set_name(n);
}

void* ClientTimestampFiller::run() {
	// Remove 1 item at a time and process it. Blocks if no items are available to process.
	for (int i = 0;; i++) {
		long long timestamp = m_queue.remove();

		//Update client_timestamp.txt with the timestamp
		std::ofstream file;
		file.open("client_timestamp.txt", std::ofstream::app);
		if (file.is_open()){
			std::string nl = "\n";
			file << timestamp;
			file << nl;
			file.close();
		}
		else {
			std::cout << "Error opening file";
		}
		

	}

	//Should not get here
	return NULL;
}