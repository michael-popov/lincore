/**********************************************
   File:   url_encoder.h

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

#ifndef CDB_UTILS_URL_ENCODER_H
#define CDB_UTILS_URL_ENCODER_H

#include <string>

using std::string;

namespace cdb {

string encode(const string& str);
string decode(const string& str);

} // namespace cdb

#endif // CDB_UTILS_URL_ENCODER_H
