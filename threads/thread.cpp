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
*/

#include "thread.h"

static void* runThread(void* arg)
{
    return ((Thread*)arg)->run();
}

/**
 * Constructor for the Thread class. By default, sets the values of
   thread id(m_tid), if the thread is detatched (m_running), and if the thread is
   detached (m_detached) all to 0.
 */
Thread::Thread() : m_tid(0), m_running(0), m_detached(0){}

/**
 * Destructor for the Thread class.
   If the Thread is running but not detached, calls pthread_detach to detatch the Thread
   If the Thread is running, calls pthread_cancel to cancel the thread
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

/**
  * Starts the Thread by calling pthread_create
 */
int Thread::start()
{
    int result = pthread_create(&m_tid, NULL, runThread, this);
    if (result == 0) {
        m_running = 1;
    }
    return result;
}

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

pthread_t Thread::self() {
    return m_tid;
}

std::string Thread::thread_name() {
	return name;
}

void Thread::set_name(std::string n) {
	name = n;
}
