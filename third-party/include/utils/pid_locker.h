/**********************************************
   File:   pid_locker.h

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

#ifndef CDB_UTILS_PID_LOCKER_H
#define CDB_UTILS_PID_LOCKER_H

#include <boost/interprocess/sync/file_lock.hpp>

namespace cdb {

class PidLocker
{
private:
    static const char* FILE_NAME;

public:
    PidLocker(const char* fileName = FILE_NAME);
    ~PidLocker();

private:
    boost::interprocess::file_lock* m_lock;
};

} // namespace cdb

#endif // CDB_UTILS_PID_LOCKER_H
