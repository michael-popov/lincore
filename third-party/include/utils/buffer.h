/**********************************************
   File:   buffer.h

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

#ifndef CDB_UTILS_BUFFER_H
#define CDB_UTILS_BUFFER_H

#include <string>

using std::string;

namespace cdb {

class Buffer {
public:
    Buffer();
    ~Buffer();

    void init(char* buffer, size_t size);
    void init(size_t size);
    void extend(size_t increment);
    bool fixed() { return m_fixed; }

    size_t length() { return (m_length < m_offset) ? 0 : m_length - m_offset; }
    const char* data();
    void release(size_t length) { m_offset += length; if (m_offset >= m_length) m_offset = m_length = 0; }

    void clear();
    size_t available() { return m_size - m_length; }
    char* buffer();
    void append(const string& content);
    void append(const char* buf, size_t size);
    void commit(size_t length);
    void closeText();

    char* raw() { return m_buffer; }
    void setLength(size_t length) { m_length = length; }
    size_t size() { return m_size; }

    void shiftContent();

private:
    char*  m_buffer;
    size_t m_size;
    size_t m_length;
    size_t m_offset;
    bool   m_fixed;
};


} // namespace cdb

#endif // CDB_UTILS_BUFFER_H
