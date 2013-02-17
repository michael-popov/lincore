/**********************************************
   File:   config.h

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

#ifndef CDB_UTILS_CONFIG_H_
#define CDB_UTILS_CONFIG_H_

#include <map>
#include <string>

using std::string;
using std::map;

#define CONFIG_HOME_DIR       "home"
#define CONFIG_CONNECT_PORT   "port"
#define CONFIG_CONNECT_HOST   "host"
#define CONFIG_LOG_LEVEL      "log"
#define CONFIG_ALLOW_DOWNLOAD "allow_download"

#define CONFIG_PREPARE_ONCE   "initialize"
#define INITIALIZE_ONCE_VALUE "CLEAN_INSTALL"

#define DATA_DIR              "data"

#define DEFAULT_LOG_LEVEL     "INFO"
#define DEFAULT_HOST          "localhost"
#define DEFAULT_PORT          1998

namespace cdb {

class Config
{
private:
    Config() {}
    Config( const Config& );
    Config& operator=( const Config& );

public:
    static Config& instance();
    static const char* version();

public:
    void process( int argc, char* argv[] );
    bool isSet( const string& name );
    bool get( const string& name, int& target );
    bool get( const string& name, short& target );
    bool get( const string& name, string& target );
    void descr(map< string, string >& result, const string& pattern);

    string contentType(const string& extension);

    // keep it public for testing purposes
    void processArgument( const char* s );

private:
    typedef map< string, string > ConfigMap;
    ConfigMap m_configMap;

private:
    void processCommandLineArguments( int argc, char* argv[] );
    void processEnvArguments();

};

} // namespace cdb

#endif // CDB_UTILS_CONFIG_H_
