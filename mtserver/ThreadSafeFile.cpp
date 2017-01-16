#include "ThreadSafeFile.h"

ThreadSafeFile::ThreadSafeFile(std::ofstream& ofs) : file(ofs) {
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond_var, NULL);
}

std::ofstream& ThreadSafeFile::getFileStream() {
	return file;
}

void ThreadSafeFile::open(const char* filename) {
	pthread_mutex_lock(&lock);
	file.open(filename);
	pthread_mutex_unlock(&lock);
}

void ThreadSafeFile::read() {
	pthread_mutex_lock(&lock);

	pthread_mutex_unlock(&lock);
}

void ThreadSafeFile::write() {
	pthread_mutex_lock(&lock);

	pthread_mutex_unlock(&lock);
}

void ThreadSafeFile::close() {
	pthread_mutex_lock(&lock);
	file.close();
	pthread_mutex_unlock(&lock);
}


ThreadSafeFile::~ThreadSafeFile() {
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond_var);
}
