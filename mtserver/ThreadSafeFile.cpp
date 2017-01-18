#include "ThreadSafeFile.h"

ThreadSafeFile::ThreadSafeFile(std::ofstream& ofs) : file(ofs) {
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
	Reads every line of the file, puts them all
	into a vector, then returns that vector
*/
std::vector<std::string>& ThreadSafeFile::read() {
	pthread_mutex_lock(&lock);		//always lock before doing anything

	//Part 1: Begin read
	while (active_writers == 1 || waiting_writers > 0) {
		++waiting_readers;
		pthread_cond_wait(&read_cond_var, &lock);
		--waiting_readers;
	}

	++active_readers;
	pthread_cond_signal(&read_cond_var);

	//Part 2: The actual read operation


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
