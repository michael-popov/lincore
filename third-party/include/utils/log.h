/**********************************************
   File:   log.h

   This code is based on the sources of a log class published in Dr.Dobbs

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

#ifndef CDB_UTILS_LOG_H
#define CDB_UTILS_LOG_H

#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

enum LogLevel {LL_CRITICAL, LL_ERROR, LL_WARN, LL_INFO, LL_TRACE, LL_DEBUG};

template <typename T>
class Log
{
public:
    Log();
    virtual ~Log();
    std::ostringstream& Get( LogLevel level = LL_INFO, const char* file=0, int line=0 );
public:
    static LogLevel& ReportingLevel();
    static const char* ToString(LogLevel level);
    static LogLevel FromString(const std::string& level);
protected:
    std::ostringstream os;
private:
    Log(const Log&);
    Log& operator =(const Log&);
};

template <typename T>
Log<T>::Log()
{
}

template <typename T>
std::ostringstream& Log<T>::Get(LogLevel level, const char* file, int line )
{
    char header[256]  = { '\0' };
    char str_time[64] = { '\0' };
    char str_date[64] = { '\0' };

#ifdef WIN32
    GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0, "HH:mm:ss", str_time, sizeof(str_time));
    GetDateFormatA(LOCALE_USER_DEFAULT, 0, 0, "yyyy:MM:dd", str_date, sizeof(str_date));
    _snprintf_s(header, sizeof(header), sizeof(header), "%u  %s  %s  ", GetProcessId(GetCurrentProcess()), str_date, str_time);
#else
    time_t t;
    tm r = {0};
    time(&t);
    localtime_r(&t, &r);
    strftime(str_time, sizeof(str_time), "%X", localtime_r(&t, &r));
    strftime(str_date, sizeof(str_date), "%F", localtime_r(&t, &r));
    snprintf(header, sizeof(header), "%d  %s  %s  ", getpid(), str_date, str_time);
#endif

    os << header << ToString(level);
    if (file) os << file << ":" << line << "\t";

    return os;
}

template <typename T>
Log<T>::~Log()
{
    os << std::endl;
    T::Output(os.str());
}

template <typename T>
LogLevel& Log<T>::ReportingLevel()
{
    static LogLevel level = LL_INFO;
    return level;
}

template <typename T>
const char* Log<T>::ToString(LogLevel level)
{
	static const char* const buffer[] = {
        "CRITICAL ",
        "ERROR    ", 
        "WARN     ", 
        "INFO     ", 
        "TRACE    ", 
        "DEBUG    " 
    };
    return ((size_t)level < sizeof(buffer)/sizeof(*buffer)) ? buffer[level] : "UNKNOWN";
}

template <typename T>
LogLevel Log<T>::FromString(const std::string& level)
{
    if (level == "DEBUG") return LL_DEBUG;
    if (level == "TRACE") return LL_TRACE;
    if (level == "INFO") return LL_INFO;
    if (level == "WARN") return LL_WARN;
    if (level == "ERROR") return LL_ERROR;
    if (level == "CRITICAL") return LL_CRITICAL;
    return LL_INFO;
}

class OutputStderr
{
public:
    static void Output(const std::string& msg) {
#ifdef WIN32
        DWORD written;
        WriteFile(GetStdHandle(STD_ERROR_HANDLE), msg.c_str(), msg.length(), &written, NULL);
#else
        write( 2, msg.c_str(), msg.length());
#endif
    }
};

class FILELog : public Log<OutputStderr> {};

#define FILE_LOG(level,file,line) \
    if (level > FILELog::ReportingLevel()) ; \
    else FILELog().Get(level)

#define LOG_CRITICAL  FILE_LOG(LL_CRITICAL,0,0)
#define LOG_ERROR     FILE_LOG(LL_ERROR,0,0)
#define LOG_WARN      FILE_LOG(LL_WARN,0,0)
#define LOG_INFO      FILE_LOG(LL_INFO,0,0)
#define LOG_TRACE     FILE_LOG(LL_TRACE,0,0)
#define LOG_DEBUG     FILE_LOG(LL_DEBUG,__FILE__,__LINE__)

#endif //CDB_UTILS_LOG_H
