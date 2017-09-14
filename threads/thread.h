/*!
   
   This class is from the base implementation by Vic Hargrave
   
   thread.h

   Header for a Java style thread class in C++.

   ------------------------------------------

   Copyright (c) 2013 Vic Hargrave

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef __thread_h__
#define __thread_h__

#include <pthread.h>
#include <string>

class Thread
{
  public:
	/**
	 * Constructor for thread class
	   By default, sets the values of thread id(m_tid), if the thread is detatched (m_running), 
	   and if the thread is detached (m_detached) all to 0.
	*/
    Thread();	

	/**
	 * Destructor for thread class
	   If the Thread is running but not detached, calls pthread_detach to detatch the Thread
	   If the Thread is running, calls pthread_cancel to cancel the thread
	*/
    virtual ~Thread();

	/**
	 * Starts the Thread by calling pthread_create()	   
	*/
    int start();

	/**
	 *
	*/
    int join();
    int detach();
    pthread_t self();
	std::string thread_name();
	void set_name(std::string n);
    
    virtual void* run() = 0;
    
  private:
    pthread_t  m_tid;			/*!<id of thread*> */
    int        m_running;		/*!<Whether the thread is running or not> */
    int        m_detached;		/*!<Whether the thread is detached or not> */
	std::string	   name;		/*!<The name of the thread> */
};

#endif
