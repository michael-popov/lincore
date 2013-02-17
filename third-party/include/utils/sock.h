/**********************************************
   File:   sock.h

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

#ifndef CDB_UTILS_SOCK_H
#define CDB_UTILS_SOCK_H

#ifdef WIN32
#include <Windows.h>
typedef SOCKET SOCK;
#else
typedef int SOCK;
#endif

#include <stddef.h>


namespace cdb {

class Sock
{
public:
    static void init();

public:
    Sock();
    explicit Sock(SOCK socket) : m_socket(socket), m_selectable(false) {}
    ~Sock();

    void connect(const char* addr, short port);
    void disconnect();

    void listen(const char* addr, short port);
    Sock* accept();

    int receive(char* buf, int length);
    int send(const char* buf, int length);

    void set(SOCK value) { m_socket = value; }
    SOCK get() const { return m_socket; }

    void setSelectable() { m_selectable = true; }

private:
    SOCK m_socket;
    bool m_selectable;

};

} // namespace cdb

#endif // CDB_UTILS_SOCK_H
