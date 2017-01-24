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

	The Readers-Writers problem:
		-Needs to provide a balance between readers and writers
		-Multiple readers can read at a time, but only 1 writer can write 
		 at a time
		1) Rules:
			1. Readers can only read when there's no writers
			2. Writers can only write when there's no active readers 
			3. Writer can't enter if 
		2) Reader:
			-made of 3 parts
				-Begin Read
				-<Actual reading operation>
				-End read
			1) Begin read:
				- If there is an active writer, or waiting writers, increment
				  # of waiting readers, then wait() in a while loop for the
				  read condvar. After wait() unblocks, decrement # of waiting
				  readers, as you're now active. However, if there's still an
				  active writer, or waiting writers, go back into the loop.
				- Increment the # of active readers
				- Signal the read condvar, as if one reader can get in, all
				  the other readers should be let in too.
			2) Read operation:
			3) End read:
				- If after we decrement the # of active readers, it's 0, 
				  signal the write condvar.
				- AKA, if there are no more active readers, let a writer in.
		3) Writer:
			-made of 3 parts
				-Begin write
				-<Actual write operation>
				-End write
			1) Begin write:
				- If there is an active writer, or active readers, increment
				  # of waiting writers, then wait() in the while loop. 
				  a while loop for the write condvar. After wait() unblocks, 
				  decrement # of waiting writers, as you're now active. 
				  However, if there's still an active writer, or waiting 
				  readers, go back into the loop.
				- Then, set the # of writers to be 1.
			2) Write operation:
			3) End write:
				- Set # of active writers to 0
				- If there are still waiting readers, signal the read condvar
				  to wake up any waiting readers.
				- Else, signal the write condvar to wake up any waiting writers.
				

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
		pthread_cond_t   read_cond_var;		//Condition variable, to signal a waiting Thread that they can now use the file
		pthread_cond_t   write_cond_var;	//Condition variable, to signal a waiting Thread that they can now use the file

		int active_readers;				//# Threads actively reading the file
		int active_writers;				//# Threads actively writing to the file
		int waiting_readers;			//# Threads waiting to read the file
		int waiting_writers;			//# Threads waiting to write to the file


	public:
		ThreadSafeFile(std::string n);
		std::string getFileName();
		void read(std::vector<std::string>& messages, long timestamp);
		void write(std::string item);
		~ThreadSafeFile();
};
