/**********************************************
   File:   client.h

   Copyright 2013 Michael Popov

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

#ifndef CLIENT_H
#define CLIENT_H

#include "metrics_data.h"
#include "utils/sock.h"
#include <string>
#include <list>

using std::string;
using std::list;

namespace lincore {

class Client
{
public:
    Client() : m_connected(false), m_test(false) {}

    void init();

    void createSchema(list< MetricInfo >& info);
    void setStaticData(const string& data);
    void startStreaming(const string& header = "");
    void send(const string& line);

    string host() { return m_host; }
    short int port() { return m_port; }

private:
    cdb::Sock m_sock;
    bool m_connected;
    bool m_test;
    string m_dataspace;
    string m_collection;
    string m_host;
    short  m_port;

private:
    void connect();

};

} // namespace lincore

#endif // CLIENT_H
