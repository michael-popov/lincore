/**********************************************
   File:   metrics_data.h

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

#ifndef METRICS_DATA_H
#define METRICS_DATA_H

#include "sigar_iface.h"
#include <string>
#include <map>
#include <list>

using std::string;
using std::map;
using std::list;

namespace lincore {

struct Metric
{
    int m_rate;
    double* m_data;
    string m_type;
    bool m_integer;
};

struct MetricInfo
{
    string m_name;
    string m_type;
};

typedef map< string, Metric > MetricsMap;

class MetricsData
{
public:
    ~MetricsData();

    void init();
    void uninit();

    void getMetricsInfo(list< MetricInfo >& info);

    string getStreamTitle();
    string getStaticMetrics();
    string getStreamMetrics(int ts);

    void collectInitial();
    void collect();

private:
    SigarIface m_sigar;

    MetricsMap m_metrics;
    
    LoadAverages m_lavgs;
    Memory m_memory;
    Swap m_swap, m_swapCache;
    Disk m_disk, m_diskCache;
    CPU m_cpu;
    CPUPercent m_cpuPercent;
    ProcessCount m_processCount;
    double m_coresCount;
    map< string, FSInfo* > m_fs;
    map< string, Disk* > m_disks;
    map< string, Disk > m_disksCache;
    map< string, NetMetrics* > m_nets;
    map< string, NetMetrics > m_netsCache;
    Tcp m_tcp;

private:
    void fillMetrics();
    void fillDisks();
    void fillNets();
    void fillFS();
    void filterMetrics();
    void calcSize();
};

} //namespace lincore

#endif // METRICS_DATA_H
