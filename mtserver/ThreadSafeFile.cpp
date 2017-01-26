#include "ThreadSafeFile.h"
#define NEWLINE_ASCII 10

ThreadSafeFile::ThreadSafeFile(std::string n) 
	: name(n)
{
	//Initialize locks and condition variables
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&read_cond_var, NULL);
	pthread_cond_init(&write_cond_var, NULL);

	//Intialize fields
	active_readers = 0;
	active_writers = 0;
	waiting_readers = 0;
	waiting_writers = 0;
}

/*
	Takes in a reference to a vector, then fills it with strings.
	This way, we don't have to explicitly return.

	Case 1: Reading timestamp file
	Start at EOF, then read backwards one line
	
	Case 2: Reading master log
	Reads every line of the file, and if the timestamp of
	the line is greater than the timestamp in the parameter,
	adds the line into a vector. 
	At the end, returns that vector with all the appropriate 
	strings.
*/
void ThreadSafeFile::read(std::vector<std::string>& messages, long timestamp) {
	pthread_mutex_lock(&lock);			//always lock before doing anything

	//Part 1: Begin read
	while (active_writers == 1 || waiting_writers > 0) {
		++waiting_readers;
		pthread_cond_wait(&read_cond_var, &lock);
		--waiting_readers;
	}

	++active_readers;
	pthread_cond_signal(&read_cond_var);

	//Part 2: The actual read operation
	
	//Case 1: Reading master log
	if (!name.compare("master_log.txt") ) {
		std::string delimiter = ":::::::";
		std::string line;
		std::ifstream file("master_log.txt");
		if (file.is_open()) {
			while (getline(file, line)) {	////getline() grabs the string up to "\n"
				//std::cout << "Masterlog current line: " << line << std::endl;
				int c = line.at(line.length() - 1);
				//std::cout << "Last character in line: " << c << std::endl;

				//Extract the current timestamp out of the message
				int delimiter_pos = line.find(delimiter);
				std::string token = line.substr(0, delimiter_pos);
				char* sz;   // alias of size_t
				long curr_timestamp = std::strtol(token.c_str(), &sz, 10);
				
				//If the timestamp >= timestamp from the client, put the line
				//into the vector
				if (curr_timestamp >= timestamp) {
					messages.push_back(line);
				}

			}
		}

	}

	//Case 2: Reading timestamp file
	else {
		//First, read in 1 line of input backwards, one char at a time
		//We'll know it's a next line when the second NL line feed (new line)
		//shows up, which has an ascii value of 10.
		std::string raw;
		char c;
		int num_new_lines = 0;
		bool still_one_line = true;
		int i = 1;

		std::ifstream file(name.c_str(), std::ios::ate);
		std::streampos size = file.tellg();
		while(still_one_line && i < size+1) {
			file.seekg(-i, std::ios::end);
			file.get(c);
			//printf("%c, ", c);
			//printf("int rep: %d\n", c);
			//If we encounter a newline char, increment
			if (c == NEWLINE_ASCII) {
				++num_new_lines;
			}

			//It's the next line, so stop
			if (num_new_lines == 2) {
				still_one_line = false;
			}

			//If we didn't encounter a new line
			else {
				raw += c;
			}

			++i;
		}

		//Then, reverse the string, so it's in the correct order
		std::reverse(raw.begin(), raw.end());

		std::cout << "read timestamp: " << raw << std::endl;
		//Then, put the string into the vector, which we'll return
		messages.push_back(raw);

		//Close file
		file.close();
	}

	//Part 3: End read
	if (--active_readers == 0) {
		pthread_cond_signal(&write_cond_var);
	}

	pthread_mutex_unlock(&lock);	//always unlock after doing operations
}

/*
	Writes the string to the end of the file
*/
void ThreadSafeFile::write(std::string item) {
	pthread_mutex_lock(&lock);		//always lock before doing anything

	//Part 1: Begin write
	while (active_writers == 1 || waiting_readers > 0) {
		++waiting_writers;
		pthread_cond_wait(&write_cond_var, &lock);
		--waiting_writers;
	}

	//++active_writers;
	active_writers = 1;		//set to 1, as there can only be 1 writer
							//at a time
	
	//Part 2: The actual write operation
	std::ofstream file(name.c_str(), std::ofstream::app);		//app = append
	if (file.is_open()) {
		std::string nl = "\n";
		file << item;
		file << nl;
	}

	//Part 3: End write
	//--active_writers;		
	active_writers = 0;		//set back to 0

	//If there are waiting readers, signal one of them
	if (waiting_readers) {	
		pthread_cond_signal(&read_cond_var);
	}

	//Else, signal a waiting writer
	else {
		pthread_cond_signal(&write_cond_var);
	}

	pthread_mutex_unlock(&lock);	//always unlock after doing operations;
}

ThreadSafeFile::~ThreadSafeFile() {
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&read_cond_var);
	pthread_cond_destroy(&write_cond_var);
}
