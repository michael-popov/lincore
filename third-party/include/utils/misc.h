/**********************************************
   File:   misc.h

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

#ifndef CDB_UTILS_MISC_H
#define CDB_UTILS_MISC_H

#include <string>
#include <sstream>

#define NO_COPIES(T)    T(const T&); void operator=(const T&)

namespace cdb {

template< typename T >
static inline
std::string toString(T v)
{
    std::ostringstream ostr;
    ostr << v;
    return ostr.str();
}


std::string getSystemError();

} // namespace cdb


#endif // CDB_UTILS_MISC_H
