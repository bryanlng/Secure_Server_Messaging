#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
/*
	Abstraction over C++'s file operations that allows multiple threads to 
	open, read, write, and close the file


	Basically, this is the readers writers problem.

	Partial solution provided at:
	http://tinyurl.com/rwsolution
	However, I modified it so that we WAIT IN WHILE LOOPS, which you MUST do
	in order to ensure that we get the correct results, as we could have been
	robbed along the way.

	The Readers-Writers problem (Norman's solution):

		

	Example case scenarios:
	1. 10 readers come in, then a writer:
		-10 readers get in
		-writer waits. And waits. And waits. Until the last reader calls
		 signal(canWrite)
	2. 2 readers come in, then 2 writers at almost the exact same time:
		-2 readers get in
		-Writer 1 gets into the if conditional:
			- now 1 waiting writer
			- writer 1 wait()
		-Writer 2 gets into the if conditional:
			- now 2 waiting writers
			- writer 2 wait()
		-Both readers exit --> writer 1 gets out of wait(), does its write()
	     stuff, then signals writer 2, since there's no more waiting readers.
		-Waiter 2 does basically the same thing as writer 1 did above
		


	http://www.cplusplus.com/doc/tutorial/files/
	http://stackoverflow.com/questions/9658720/ofstream-as-function-argument
	http://stackoverflow.com/questions/7868936/read-file-line-by-line
	http://stackoverflow.com/questions/15822062/c-reading-file-backwards-from-the-end-of-the-file
	http://stackoverflow.com/questions/29495546/reading-file-backwards-c-ifstream
	http://stackoverflow.com/questions/4155537/writting-into-a-text-file-without-overwritting-it
	http://www.cplusplus.com/reference/fstream/ofstream/open/
*/

class ThreadSafeFile {
	private:
		std::string		 name;				//name of the file. Either "master_log.txt", or "timestamp.txt"
		pthread_mutex_t  lock;				//Lock, so that only 1 Thread can edit a File at a time
		pthread_cond_t   rww_cond_var;		//Reader waiting on writers cond var
		pthread_cond_t   wwr_cond_var;		//Writers waiting on readers cond var

		int readers;
		int writers;


	public:
		ThreadSafeFile(std::string n);
		void read(std::vector<std::string>& messages, long timestamp);
		void write(std::string item);
		~ThreadSafeFile();
};
