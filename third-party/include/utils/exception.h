/**********************************************
   File:   exception.h

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

#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <string>
#include <iostream>

namespace cdb {

class BaseException
{
public:
    BaseException(const std::string& cause,
                  const std::string& sourceFile,
                  int sourceLine)
      : m_cause(cause),
        m_sourceFile(sourceFile),
        m_sourceLine(sourceLine)
    { 

    }

    virtual ~BaseException() {}

    // Accessors
    const std::string& cause() const { return m_cause; }
    const std::string& file() const { return m_sourceFile; }
    int line() const { return m_sourceLine; }

private:
    std::string m_cause;
    std::string m_sourceFile;
    int m_sourceLine;
};

class Exception : public BaseException 
{
public:
    Exception(const std::string& cause,
              const std::string& sourceFile = "",
              int sourceLine = 0)
      : BaseException(cause, sourceFile, sourceLine) 
    {}
};

inline std::ostream& operator<<(std::ostream& os, const Exception& e)
{
    os << "EXCEPTION: " << e.cause() << " (" << e.file() << ":" << e.line() << ")";
    return os;
}

class GlobalException : public BaseException 
{
    GlobalException(const std::string& cause,
                    const std::string& sourceFile = "",
                    int sourceLine = 0)
      : BaseException(cause, sourceFile, sourceLine) 
    {}
};

#define THROW(cause) throw  Exception(cause, __FILE__, __LINE__)

} // namespace cdb

#endif // EXCEPTION_H_

