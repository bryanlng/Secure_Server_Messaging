#include "ThreadSafeFile.h"
#define NEWLINE_ASCII 10

ThreadSafeFile::ThreadSafeFile(std::ofstream& ofs, std::string n) 
	: file(ofs), name(n)
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

std::ofstream& ThreadSafeFile::getFileStream() {
	return file;
}

void ThreadSafeFile::open(const char* filename) {
	pthread_mutex_lock(&lock);
	file.open(filename);
	pthread_mutex_unlock(&lock);
}

/*
	Case 1: Reading timestamp file
	Start at EOF, then read backwards one line
	
	Case 2: Reading master log
	Reads every line of the file, and if the timestamp of
	the line is greater than the timestamp in the parameter,
	adds the line into a vector. 
	At the end, returns that vector with all the appropriate 
	strings.
*/
std::vector<std::string>& ThreadSafeFile::read(long timestamp) {
	pthread_mutex_lock(&lock);			//always lock before doing anything
	std::vector<std::string> messages;	//vector to store the messages we
										//read from the file

	//Part 1: Begin read
	while (active_writers == 1 || waiting_writers > 0) {
		++waiting_readers;
		pthread_cond_wait(&read_cond_var, &lock);
		--waiting_readers;
	}

	++active_readers;
	pthread_cond_signal(&read_cond_var);

	//Part 2: The actual read operation
	//Case 1: Reading timestamp file
	if (!name.compare("timestamp.txt")) {

		//First, read in 1 line of input backwards, one char at a time
		//We'll know it's a next line when the second NL line feed (new line)
		//shows up, which has an ascii value of 10.
		std::string raw;
		char c;
		int num_new_lines = 0;
		bool still_one_line = true;
		int i = 1;

		std::ifstream myFile("timestamp.txt", std::ios::ate);
		std::streampos size = myFile.tellg();
		while(still_one_line && i < size) {
			myFile.seekg(-i, std::ios::end);
			myFile.get(c);
			printf("%c\n", c);
			
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

		//Then, put the string into the vector, which we'll return
		messages.push_back(raw);
	}

	//Case 2: Reading master log
	else {
		
	}

	//Part 3: End read
	if (--active_readers == 0) {
		pthread_cond_signal(&write_cond_var);
	}

	pthread_mutex_unlock(&lock);	//always unlock after doing operations
	return messages;
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

void ThreadSafeFile::close() {
	pthread_mutex_lock(&lock);
	file.close();
	pthread_mutex_unlock(&lock);
}


ThreadSafeFile::~ThreadSafeFile() {
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&read_cond_var);
	pthread_cond_destroy(&write_cond_var);
}
