/*
   thread.cpp

   Definition of a Java style thread class in C++.

   ------------------------------------------

   Copyright (c) 2013  Vic Hargrave

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   http://man7.org/linux/man-pages/man3/pthread_detach.3.html
   http://stackoverflow.com/questions/22427007/difference-between-pthread-exit-pthread-join-and-pthread-detach
   http://stackoverflow.com/questions/3438536/when-to-use-pthread-cancel-and-not-pthread-kill
   http://man7.org/linux/man-pages/man3/pthread_cancel.3.html

   Explanations of C Thread methods:
   1. pthread_detach()
	-simply MARKS the thread as "detached"
	-When a detached thread terminates ==> resources are automatically released
	 without needing for another thread to join with it
	-double detach --> bad
	
   2. pthread_join()
	-similar to waitpid
	-Wait for a certain Thread to terminate, then get its return value
	-Can be called either BEFORE or AFTER the thread you're waiting for dies
		-1) If called before it dies: 
			-Calling thread waits for the thread to die, then gets its
			 return value
		-2) If called after it dies:
			-Simply obtain value, then release its resources

   3. pthread_cancel(thread t)
	-sends a cancellation request to the thread t

*/

#include "thread.h"

/*
	calls run on the Thread
	Cast the void* to be Thread*,
	so that we can call run() on it using ->
*/
static void* runThread(void* arg)
{
    return ((Thread*)arg)->run();
}

/*
	Constructor
	At default, set all values to be 0
*/
Thread::Thread() : m_tid(0), m_running(0), m_detached(0) {}

/*
	Destructor
	1. If Thread is running but not detached ==> detach it
	2. If Thread is running but detached ==> 
*/
Thread::~Thread()
{
    if (m_running == 1 && m_detached == 0) {
        pthread_detach(m_tid);
    }
    if (m_running == 1) {
        pthread_cancel(m_tid);
    }
}

/*
	Creates a thread, and sets m_tid to be the tid of
	the newly created thread.
	Also sets running flag to be 1, if creation successful
*/
int Thread::start()
{
    int result = pthread_create(&m_tid, NULL, runThread, this);
    if (result == 0) {
        m_running = 1;
    }
    return result;
}

/*
	
*/
int Thread::join()
{
    int result = -1;
    if (m_running == 1) {
        result = pthread_join(m_tid, NULL);
        if (result == 0) {
            m_detached = 0;
        }
    }
    return result;
}

/*
	Detaches a Thread, when the caller doesn't want to wait
	for the thread to complete
*/
int Thread::detach()
{
    int result = -1;
    if (m_running == 1 && m_detached == 0) {
        result = pthread_detach(m_tid);
        if (result == 0) {
            m_detached = 1;
        }
    }
    return result;
}

/*
	Returns the tid of the thread
*/
pthread_t Thread::self() {
    return m_tid;
}
