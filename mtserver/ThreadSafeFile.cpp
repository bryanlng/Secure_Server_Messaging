#include "ThreadSafeFile.h"

ThreadSafeFile::ThreadSafeFile(ofstream ofs) : file(ofs) {
	pthread_mutex_init(&lock, NULL);
	pthread_cond_init(&cond_var, NULL);
}
ThreadSafeFile::~ThreadSafeFile() {
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&cond_var);
}
ofstream ThreadSafeFile::getFileStream() {
	return file;
}
void ThreadSafeFile::open() {

}