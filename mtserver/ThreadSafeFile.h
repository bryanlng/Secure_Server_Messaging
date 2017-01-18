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
		-Rules:
			1. Readers can only read when there's no writers
			2. Writers can only write when there's no active readers 
			3. Writer can't enter if 

	Example case scenarios:
	1. 10 readers come in, then a writer:
		-10 readers get in
		-writer waits. And waits. And waits. Until the last reader calls
		 signal(canWrite)
	2. 2 readers come in, then 2 writers:
		
		


	http://www.cplusplus.com/doc/tutorial/files/
	http://stackoverflow.com/questions/9658720/ofstream-as-function-argument
	http://stackoverflow.com/questions/7868936/read-file-line-by-line

*/


#include <fstream>
class ThreadSafeFile {
	private:
		std::ofstream&	 file;			//the ofstream that represents the file
		pthread_mutex_t  lock;			//Lock, so that only 1 Thread can edit a File at a time
		pthread_cond_t   read_cond_var;	//Condition variable, to signal a waiting Thread that they can
										//now use the file
		pthread_cond_t   write_cond_var;	//Condition variable, to signal a waiting Thread that they can
										//now use the file

		int active_readers;				//used so that we can have a condition to wait on
		int active_writers;				//used so that we can have a condition to wait on
		int waiting_readers;
		int waiting_writers;


	public:
		ThreadSafeFile(std::ofstream& ofs);
		~ThreadSafeFile();
		std::ofstream& getFileStream();
		void open(const char* filename);
		void read();
		void write();
		void close();
};
