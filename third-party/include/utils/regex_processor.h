/**********************************************
   File:   regex_processor.h

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

#ifndef REGEX_PROCESSOR_H
#define REGEX_PROCESSOR_H

#include <string>

namespace cdb {

class Regex
{
private:
    Regex(const Regex&);
    Regex& operator=(const Regex&);
public:
    Regex() : m_regex(0) {}
    ~Regex();
    void compile(const std::string& rx, bool convert = true);
    static std::string convertPattern(const std::string& pattern);
    bool match(const std::string& str);

public:
    static void initNamePattern(const std::string& rx);
    static bool matchName(const std::string& name);

private:
	void* m_regex;

private:
    static Regex m_namePattern;
};

} // namespace cdb

#endif // REGEX_PROCESSOR_H

