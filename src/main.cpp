/**********************************************
   File:   main.cpp

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

#include "metrics_data.h"
#include "client.h"
#include "utils/config.h"
#include "utils/log.h"
#include "utils/exception.h"
#include "utils/pid_locker.h"
#include <iostream>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

using std::cerr;
using std::endl;


using namespace lincore;
using namespace cdb;

static MetricsData g_metricsData;
static Client g_client;
static const char* SIGNAL_MESSAGE = "lincore exit on signal\n";
static const char* FILE_LOCK = "lincore.pid";
static bool g_keepGoing = true;

static void signalHandler(int)
{
    write(2, SIGNAL_MESSAGE, strlen(SIGNAL_MESSAGE));
    g_keepGoing = false;
}

void doWork(list< MetricInfo >& info)
{
    LOG_INFO << "Connecting to " << g_client.host() << ":" << g_client.port();

    g_client.createSchema(info);

    g_metricsData.collectInitial();
    string staticMetrics = g_metricsData.getStaticMetrics();
    if (!staticMetrics.empty()) {
        g_client.setStaticData(staticMetrics);
    }

    g_client.startStreaming(g_metricsData.getStreamTitle());

    while (g_keepGoing) {
        sleep(1);
        if (!g_keepGoing) break;

        int t = (int) time(NULL);
        g_metricsData.collect();
        g_client.send(g_metricsData.getStreamMetrics(t));
    }
}

void exceptionalMain(int argc, char* argv[])
{
    Config& config = Config::instance();
    config.process(argc, argv);

    string logLevel = DEFAULT_LOG_LEVEL;
    config.get(CONFIG_LOG_LEVEL, logLevel);
    FILELog::ReportingLevel() = FILELog::FromString(logLevel);

    LOG_INFO << "Start ClockWorkdDB daemon";

    PidLocker pidLocker(FILE_LOCK);

    signal( SIGINT, signalHandler );
    signal( SIGTERM, signalHandler );

    g_client.init();

    list< MetricInfo > info;
    g_metricsData.init();
    g_metricsData.getMetricsInfo(info);
    
    while (g_keepGoing) {
        try {
            doWork(info);
        }
        catch(Exception&) {
            sleep(1);
        }
    }

    g_metricsData.uninit();
}

int main(int argc, char* argv[])
{
    int ret = 0;
    try {
        exceptionalMain(argc, argv);
    }
    catch(Exception& e) {
        cerr << e << endl;
        ret = -1;
    }
    catch(...) {
        cerr << "Unidentified exception" << endl;
        ret = -1;
    }
    return ret;
}

