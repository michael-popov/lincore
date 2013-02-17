/**********************************************
   File:   client.cpp

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


#include "client.h"
#include "utils/config.h"
#include "utils/exception.h"
#include "utils/log.h"
#include <iostream>

using std::cout;
using std::endl;

using namespace cdb;

namespace lincore {

void Client::init() 
{ 
    int testMode = 0;
    Config::instance().get("test", testMode);
    m_test = (testMode != 0);

    if (!m_test) {
        m_port = -1;
        Config::instance().get("host", m_host);
        Config::instance().get("port", m_port);
        if (m_host.empty() || (m_port < 0)) THROW("Invalid connection parameters");
    }

    Config::instance().get("dataspace", m_dataspace);
    Config::instance().get("collection", m_collection);
    if (m_dataspace.empty() || m_collection.empty()) {
        THROW("CDB schema configuration is not defined");
    }
}

void Client::createSchema(list< MetricInfo >& info)
{
    string cmd;
    cmd = string("create dataspace ") + m_dataspace + " with ifexists=ignore";
    send(cmd);
    cmd = string("create collection ") + m_dataspace + "." + m_collection + " with ifexists=ignore";
    send(cmd);
    for (list< MetricInfo >::iterator iter = info.begin(); iter != info.end(); ++iter) {
        string metric = iter->m_name;
        string type = iter->m_type;
        cmd = string("create metric ") + m_dataspace + "." + m_collection +  "." + metric + 
                 " with ifexists=ignore, type=" + type;
        send(cmd);
    }
}

void Client::setStaticData(const string& data)
{
    string cmd = string("set ") + m_dataspace + "." + m_collection + " " + data;
    send(cmd);
}

void Client::startStreaming(const string& header)
{
    string cmd = string("insert ") + m_dataspace + "." + m_collection + " " + header;
    send(cmd);
}

void Client::connect()
{
    if (m_test) {
        m_connected = true;
        return;
    }

    static const char* PUT_COMMAND = "PUT\n";
    if (!m_connected) {
        try {
            m_sock.connect(m_host.c_str(), m_port);
            m_sock.send(PUT_COMMAND, 4);
        }
        catch(Exception& e) {
            LOG_ERROR << "Failed to connect to " << m_host << ":" << m_port;
            throw;
        }

        m_connected = true;
    }
}

void Client::send(const string& line)
{
    string fullCmd = line + "\n";

    if (m_test) {
        cout << fullCmd;
        cout.flush();
    }

    connect();

    try {
        m_sock.send(fullCmd.c_str(), fullCmd.length());
    }
    catch(Exception& e) {
        LOG_ERROR << "Failed to send data " << e.cause();
        m_sock.disconnect();
        m_connected = false;
        throw;
    }
}

} // namespace lincore

