#include <fstream>
class ThreadSafeFile {
	private:
		ofstream file;	//the ofstream that represents the file

	public:
		ofstream getFileStream();
		void open();
};
