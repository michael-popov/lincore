/**********************************************
   File:   queue.h

   This code is based on sources of Queue class published on-line.

   Copyright 2012 Michael Popov

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 **********************************************/

#ifndef CDB_SYSTEM_QUEUE_H
#define CDB_SYSTEM_QUEUE_H

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;

namespace cdb {

class Queue {
public:
    Queue() : m_start(0), m_count(0) { m_tasks.resize(QUEUE_SIZE, 0); }

    bool put(void* task) {
        if (m_count >= QUEUE_SIZE) return false;
        boost::mutex::scoped_lock guard(m_mutex);
        m_tasks[(m_start + m_count) % QUEUE_SIZE] = task;
        m_count++;
        m_cond.notify_one();
        return true;
    }

    void* get() {
        boost::mutex::scoped_lock guard(m_mutex);
        while (m_count == 0) m_cond.wait(guard);
        void* task = m_tasks[m_start];
        m_start = (m_start + 1) % QUEUE_SIZE;
        m_count--;
        return task;
    }

private:
    static const size_t QUEUE_SIZE = 1024;

private:
    size_t m_start;
    size_t m_count;
    boost::mutex m_mutex;
    boost::condition m_cond;
    std::vector<void*> m_tasks;
};

} // namespace cdb

#endif // CDB_SYSTEM_QUEUE_H
