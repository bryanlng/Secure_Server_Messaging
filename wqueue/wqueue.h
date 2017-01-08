/*
   wqueue.h

   Worker thread queue based on the Standard C++ library list
   template class.

   Uses a C++ list (doubly-linked list) as the primary data structure
   behind the queue

   Based on the Producer-Consumer model:
	1 Thread (producer thread) places work items in a queue, while
	other threads (consumer threads) wait for and remove the items

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

   OS <====> Real world C++ usage:
   signal() -->		pthread_cond_signal() 
   broadcast() -->	pthread_cond_broadcast()
   wait()	-->		pthread_cond_wait
*/

#ifndef __wqueue_h__
#define __wqueue_h__

#include <pthread.h>
#include <list>

using namespace std;

template <typename T> class wqueue		//T is determined at runtime
{
    list<T>          m_queue;		//list
    pthread_mutex_t  m_mutex;		//Lock
    pthread_cond_t   m_condv;		//Condition variable to signal

  public:
	/*
		Constructor
		Initialize lock and condition variable
	*/
    wqueue() {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_cond_init(&m_condv, NULL);
    }

	/*
		Destructor
		Destroy lock and condition variable
	*/
    ~wqueue() {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_condv);
    }

	/*
		Adds an item to the list
		Always remember to lock before anything, then
		unlock after all operations are done 
		Steps:
			1. Push item to back of list
			2. Signal at least 1 Thread that's waiting
	*/
    void add(T item) {
        pthread_mutex_lock(&m_mutex);
        m_queue.push_back(item);
        pthread_cond_signal(&m_condv);		//signal()
        pthread_mutex_unlock(&m_mutex);		
    }

	/*
		Removes an item from the list
		Always remember to lock before anything, then
		unlock after all operations are done
		Steps:
		1. While size is 0, call wait().
			-Always need to wait() in a while loop, as
			 we may have been robbed along the way
		2. Get the first item in the queue, then remove it
		   from the queue
	*/
    T remove() {
        pthread_mutex_lock(&m_mutex);
        while (m_queue.size() == 0) {
            pthread_cond_wait(&m_condv, &m_mutex);
        }
        T item = m_queue.front();
        m_queue.pop_front();
        pthread_mutex_unlock(&m_mutex);
        return item;
    }

	/*
		Gets size of list
		Always remember to lock before anything, then
		unlock after all operations are done
		Steps:
			1. Simplyl call size()
	*/
    int size() {
        pthread_mutex_lock(&m_mutex);
        int size = m_queue.size();
        pthread_mutex_unlock(&m_mutex);
        return size;
    }
};

#endif
