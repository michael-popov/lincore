/**********************************************
   File:   perf.h

   Class Perf contains performance counters and
   state information. It provides efficient access
   for updaters. Content of the class can be dumped
   into a text file or into stdout

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

#ifndef PERF_H
#define PERF_H

#include <boost/thread/mutex.hpp>
#include <map>
#include <string>

using std::map;
using std::string;

namespace cdb {

class Perf
{
private:
    Perf() {}
    Perf(const Perf&);
    Perf& operator=(const Perf&);

    ~Perf();

public:
    static Perf& instance();

public:
    long* counter(const string& name);
    void inc(long* cntr, long value);  // value can be negative

    string state(const string& name);
    void set(const string& name, const string& value);

    void dump(const char* path, const char* pattern);
    void descr(map< string, string >& result, const string& pattern);

private:
    typedef map< string, long* > CountersMap;
    typedef map< string, string > StatesMap;

private:
    CountersMap   m_counters;
    StatesMap     m_states;
    boost::mutex  m_mutex;

};

} // namespace cdb

#endif // PERF_H

