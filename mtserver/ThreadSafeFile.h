#include <fstream>
class ThreadSafeFile {
	private:
		ofstream file;				//the ofstream that represents the file
		pthread_mutex_t  lock;		//Lock, so that only 1 Thread can edit a File at a time
		pthread_cond_t   cond_var;	//Condition variable, to signal a waiting Thread that they can
									//now use the file
	public:
		ThreadSafeFile(ofstream ofs);
		~ThreadSafeFile();
		ofstream getFileStream();
		void open();
};
