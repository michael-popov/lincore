/**********************************************
   File:   metrics_data.cpp

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
#include "utils/config.h"
#include "utils/exception.h"
#include "utils/log.h"
#include "utils/regex_processor.h"
#include <boost/algorithm/string.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using std::vector;
using std::ostringstream;
using std::cout;
using std::endl;

using namespace cdb;

namespace lincore {

MetricsData::~MetricsData()
{
    map< string, Disk* >::iterator iter = m_disks.begin();
    for ( ; iter != m_disks.end(); ++iter) delete iter->second;

    map< string, NetMetrics* >::iterator jter = m_nets.begin();
    for ( ; jter != m_nets.end(); ++jter) delete jter->second;

    map< string, FSInfo* >::iterator kter = m_fs.begin();
    for ( ; kter != m_fs.end(); ++kter) delete kter->second;
}

void MetricsData::init()
{
    LOG_INFO << "Initializing MetricsData";

    m_sigar.init();

    fillDisks();
    fillNets();
    fillFS();
    
    fillMetrics();
    calcSize();
    filterMetrics();
}

void MetricsData::uninit()
{
    m_sigar.uninit();
}

void MetricsData::getMetricsInfo(list< MetricInfo >& info)
{
    MetricsMap::iterator iter = m_metrics.begin();
    for ( ; iter != m_metrics.end(); ++iter) {
        MetricInfo mi = { iter->first, iter->second.m_type };
        info.push_back(mi);
    }
}

string MetricsData::getStreamTitle()
{
    ostringstream ostr;

    MetricsMap::iterator iter = m_metrics.begin();
    for (int i=0; iter != m_metrics.end(); ++iter) {
        if (iter->second.m_rate == 0) continue;

        if (i != 0) ostr << ", ";
        i++;

        ostr << iter->first << "=?";
    }

    return ostr.str();
}

string MetricsData::getStreamMetrics(int ts)
{
    ostringstream ostr;
    ostr << "_";

    MetricsMap::iterator iter = m_metrics.begin();
    for ( ; iter != m_metrics.end(); ++iter) {
        if (iter->second.m_rate == 0) continue;

        ostr << ",";
        if (ts % iter->second.m_rate != 0) continue;

        if (iter->second.m_integer) 
            ostr << std::fixed << std::setprecision(0);
        else
            ostr << std::fixed << std::setprecision(2);

        ostr << *iter->second.m_data;
    }

    return ostr.str();
}

string MetricsData::getStaticMetrics()
{
    ostringstream ostr;
    ostr << std::fixed << std::setprecision(2);

    MetricsMap::iterator iter = m_metrics.begin();
    for (int i=0; iter != m_metrics.end(); ++iter) {
        if (iter->second.m_rate != 0) continue;

        if (i != 0) ostr << ", ";
        i++;
        
        if (iter->second.m_integer) 
            ostr << std::fixed << std::setprecision(0);
        else
            ostr << std::fixed << std::setprecision(2);

        ostr << iter->first << "=" << *iter->second.m_data;
    }

    return ostr.str();
}

void MetricsData::collectInitial()
{
    int cores;
    m_sigar.getCpuCores(cores);
    m_coresCount = cores;

    m_sigar.getMemory(m_memory);

    m_sigar.getSwap(m_swap);
    m_swapCache = m_swap;

    m_sigar.getCPU(m_cpu);

    m_sigar.readDisksStats();
    m_sigar.getDisk(m_disk);
    m_diskCache = m_disk;

    {
        map< string, Disk >::iterator iter = m_disksCache.begin();
        for ( ; iter != m_disksCache.end(); ++iter) {
            m_sigar.getDisk(iter->first, iter->second);
        }
    }

    {
        map< string, NetMetrics >::iterator iter = m_netsCache.begin();
        for ( ; iter != m_netsCache.end(); ++iter) {
            m_sigar.getNet(iter->first, iter->second);
        }
    }

    {
        map< string, FSInfo* >::iterator iter = m_fs.begin();
        for ( ; iter != m_fs.end(); ++iter) {
            FSInfo* fs = iter->second;
            m_sigar.getFS(iter->first, *fs);
        }
    }
}

void MetricsData::collect()
{
    m_sigar.readDisksStats();

    m_sigar.getLoadAverages(m_lavgs);
    m_sigar.getSwap(m_swap);

    m_sigar.getMemory(m_memory);

    CPU cpu;
    m_sigar.getCPU(cpu);
    m_sigar.getCPUPercent(m_cpu, cpu, m_cpuPercent);
    m_cpu = cpu;

    Swap swap;
    m_sigar.getSwap(swap);
    m_sigar.getSwapMetricsDiff(m_swapCache, swap, m_swap);
    m_swapCache = swap;

    Disk disk;
    m_sigar.getDisk(disk);
    m_sigar.getDiskMetricsDiff(m_diskCache, disk, m_disk);
    m_diskCache = disk;

    m_sigar.getProcessCount(m_processCount);
    m_sigar.getTcp(m_tcp);

    {
        map< string, FSInfo* >::iterator iter = m_fs.begin();
        for ( ; iter != m_fs.end(); ++iter) {
            FSInfo* fs = iter->second;
            m_sigar.getFS(iter->first, *fs);
        }
    }

    {
        map< string, Disk* >::iterator iter = m_disks.begin();
        for ( ; iter != m_disks.end(); ++iter) {
            Disk disk;
            m_sigar.getDisk(iter->first, disk);

            map< string, Disk >::iterator jter = m_disksCache.find(iter->first);
            if (jter == m_disksCache.end()) THROW("Mismatch disks cache");

            m_sigar.getDiskMetricsDiff(jter->second, disk, *iter->second);
            jter->second = disk;
        }
    }

    {
        map< string, NetMetrics* >::iterator iter = m_nets.begin();
        for ( ; iter != m_nets.end(); ++iter) {
            NetMetrics netMetrics;
            m_sigar.getNet(iter->first, netMetrics);

            map< string, NetMetrics >::iterator jter = m_netsCache.find(iter->first);
            if (jter == m_netsCache.end()) THROW("Mismatch net cache");

            m_sigar.getNetMetricsDiff(jter->second, netMetrics, *iter->second);
            jter->second = netMetrics;
        }
    }
}

void MetricsData::fillDisks()
{
    string disks;
    Config::instance().get("disks", disks);
    if (disks.empty()) return;

    vector< string > v;
    boost::split(v, disks, boost::is_any_of(","));
    for (size_t i=0; i < v.size(); i++) {
        m_disks[v[i]] = new Disk;

        Disk d;
        m_disksCache[v[i]] = d;
    }
}

void MetricsData::fillNets()
{
    string nets;
    Config::instance().get("nets", nets);
    if (nets.empty()) return;

    vector< string > v;
    boost::split(v, nets, boost::is_any_of(","));
    for (size_t i=0; i < v.size(); i++) {
        m_nets[v[i]] = new NetMetrics;

        NetMetrics nm;
        m_netsCache[v[i]] = nm;
    }
}

void MetricsData::fillFS()
{
    string fs;
    Config::instance().get("fs", fs);
    if (fs.empty()) return;

    vector< string > v;
    boost::split(v, fs, boost::is_any_of(","));
    for (size_t i=0; i < v.size(); i++) {
        m_fs[v[i]] = new FSInfo;
    }

}

static Metric makeMetric(int rate, double* data, string type)
{
    bool integer = (type != "double") && (type != "float");
    Metric metric = { rate, data, type, integer };
    return metric;
}

void MetricsData::fillMetrics()
{
    m_metrics["lavg_1min"] = makeMetric(1, &m_lavgs._1min, "short");
    m_metrics["lavg_5min"] = makeMetric(60, &m_lavgs._5min, "short");
    m_metrics["lavg_15min"] = makeMetric(300, &m_lavgs._15min, "short");

    m_metrics["memory_total"] = makeMetric(0, &m_memory.total, "int");
    m_metrics["memory_used"] = makeMetric(1, &m_memory.used, "int");
    m_metrics["memory_free"] = makeMetric(1, &m_memory.free, "int");

    m_metrics["swap_total"] = makeMetric(0, &m_swap.total, "int");
    m_metrics["swap_used"] = makeMetric(1, &m_swap.used, "int");
    m_metrics["swap_free"] = makeMetric(1, &m_swap.free, "int");
    m_metrics["swap_pageIn"] = makeMetric(1, &m_swap.page_in, "int");
    m_metrics["swap_pageOut"] = makeMetric(1, &m_swap.page_out, "int");

    m_metrics["disk_reads"] = makeMetric(1, &m_disk.reads, "short");
    m_metrics["disk_writes"] = makeMetric(1, &m_disk.writes, "short");
    m_metrics["disk_readBytes"] = makeMetric(1, &m_disk.readBytes, "int");
    m_metrics["disk_writeBytes"] = makeMetric(1, &m_disk.writeBytes, "int");
    m_metrics["disk_readTime"] = makeMetric(1, &m_disk.readTime, "short");
    m_metrics["disk_writeTime"] = makeMetric(1, &m_disk.writeTime, "short");
    m_metrics["disk_totalTime"] = makeMetric(1, &m_disk.totalTime, "short");
    m_metrics["disk_waitTime"] = makeMetric(1, &m_disk.waitTime, "short");
    m_metrics["disk_queue"] = makeMetric(1, &m_disk.queue, "short");

    m_metrics["cpu_total"] = makeMetric(1, &m_cpuPercent.combined, "float");
    m_metrics["cpu_user"] = makeMetric(1, &m_cpuPercent.user, "float");
    m_metrics["cpu_system"] = makeMetric(1, &m_cpuPercent.sys, "float");
    m_metrics["cpu_nice"] = makeMetric(1, &m_cpuPercent.nice, "float");
    m_metrics["cpu_idle"] = makeMetric(1, &m_cpuPercent.idle, "float");
    m_metrics["cpu_wait"] = makeMetric(1, &m_cpuPercent.wait, "float");
    m_metrics["cpu_irq"] = makeMetric(1, &m_cpuPercent.irq, "float");
    m_metrics["cpu_softIrq"] = makeMetric(1, &m_cpuPercent.softIrq, "float");
    m_metrics["cpu_stolen"] = makeMetric(1, &m_cpuPercent.stolen, "float");

    m_metrics["process_count"] = makeMetric(1, &m_processCount.total, "short");
    m_metrics["thread_count"] = makeMetric(1, &m_processCount.threads, "short");
    m_metrics["cores_count"] = makeMetric(0, &m_coresCount, "byte");

    m_metrics["tcp_open"] = makeMetric(1, &m_tcp.connOpens, "int");
    m_metrics["tcp_fail"] = makeMetric(1, &m_tcp.connFailed, "int");
    m_metrics["tcp_count"] = makeMetric(1, &m_tcp.connections, "int");
    m_metrics["tcp_recv"] = makeMetric(1, &m_tcp.inSeg, "int");
    m_metrics["tcp_sent"] = makeMetric(1, &m_tcp.outSeg, "int");
    m_metrics["tcp_retr"] = makeMetric(1, &m_tcp.retransmits, "int");

    string prefix = "disk_";
    map< string, Disk* >::iterator iter = m_disks.begin();
    for ( ; iter != m_disks.end(); ++iter) {
        Disk* disk = iter->second;
        m_metrics[prefix+iter->first+"_reads"] = makeMetric(1, &disk->reads, "short");
        m_metrics[prefix+iter->first+"_writes"] = makeMetric(1, &disk->writes, "short");
        m_metrics[prefix+iter->first+"_readBytes"] = makeMetric(1, &disk->readBytes, "int");
        m_metrics[prefix+iter->first+"_writeBytes"] = makeMetric(1, &disk->writeBytes, "int");
        m_metrics[prefix+iter->first+"_readTime"] = makeMetric(1, &disk->readTime, "short");
        m_metrics[prefix+iter->first+"_writeTime"] = makeMetric(1, &disk->writeTime, "short");
        m_metrics[prefix+iter->first+"_queueTime"] = makeMetric(1, &disk->queueTime, "short");
        m_metrics[prefix+iter->first+"_totalTime"] = makeMetric(1, &disk->totalTime, "short");
        m_metrics[prefix+iter->first+"_queue"] = makeMetric(1, &disk->queue, "short");
    }

    prefix = "net_";
    map< string, NetMetrics* >::iterator jter = m_nets.begin();
    for ( ; jter != m_nets.end(); ++jter) {
        NetMetrics* net = jter->second;
        m_metrics[prefix+jter->first+"_rxPackets"] = makeMetric(1, &net->rxPackets, "int");
        m_metrics[prefix+jter->first+"_rxBytes"] = makeMetric(1, &net->rxBytes, "int");
        m_metrics[prefix+jter->first+"_rxErrors"] = makeMetric(1, &net->rxErrors, "int");
        m_metrics[prefix+jter->first+"_rxDropped"] = makeMetric(1, &net->rxDropped, "int");
        m_metrics[prefix+jter->first+"_rxOverruns"] = makeMetric(1, &net->rxOverruns, "int");

        m_metrics[prefix+jter->first+"_txPackets"] = makeMetric(1, &net->txPackets, "int");
        m_metrics[prefix+jter->first+"_txBytes"] = makeMetric(1, &net->txBytes, "int");
        m_metrics[prefix+jter->first+"_txErrors"] = makeMetric(1, &net->txErrors, "int");
        m_metrics[prefix+jter->first+"_txDropped"] = makeMetric(1, &net->txDropped, "int");
        m_metrics[prefix+jter->first+"_txOverruns"] = makeMetric(1, &net->txOverruns, "int");
    }

    prefix = "fs_";
    map< string, FSInfo* >::iterator kter = m_fs.begin();
    for ( ; kter != m_fs.end(); ++kter) {
        FSInfo* fs = kter->second;

        string name = kter->first;
        if (name == "/") 
            name = "";
        else {
            for (size_t i=0; i < name.length(); i++) {
                if (name[i] == '/') name[i] = '_';
            }
        }

        m_metrics[prefix+name+"_total"] = makeMetric(0, &fs->totalSpace, "int");
        m_metrics[prefix+name+"_usedPercent"] = makeMetric(10, &fs->usedPercent, "byte");
        m_metrics[prefix+name+"_free"] = makeMetric(10, &fs->freeSpace, "int");
        m_metrics[prefix+name+"_used"] = makeMetric(10, &fs->usedSpace, "int");
        m_metrics[prefix+name+"_avail"] = makeMetric(10, &fs->availSpace, "int");
    }
}

void MetricsData::filterMetrics()
{
    string filter;
    Config::instance().get("filter", filter);
    if (filter.empty()) return;

    list< Regex* > regex;
    vector< string > v;
    boost::split(v, filter, boost::is_any_of(","));
    for (size_t i=0; i < v.size(); i++) {
        regex.push_back(new Regex);
        regex.back()->compile(v[i]);
    }

    MetricsMap filtered;
    MetricsMap::iterator iter = m_metrics.begin();
    for (; iter != m_metrics.end(); ++iter) {
        list<Regex*>::iterator jter = regex.begin();
        for (; jter != regex.end(); ++jter) {
            Regex* r = *jter;
            if (r->match(iter->first)) {
                filtered[iter->first] = iter->second;
                break;
            }
        }
    }
    m_metrics.swap(filtered);

    list<Regex*>::iterator jter = regex.begin();
    for (; jter != regex.end(); ++jter) delete *jter;
}

void MetricsData::calcSize()
{
    size_t sz = 0;
    MetricsMap::iterator iter = m_metrics.begin();
    for (; iter != m_metrics.end(); ++iter) {
        if (iter->second.m_type == "byte") sz += 1;
        else if (iter->second.m_type == "short") sz += 2;
        else if (iter->second.m_type == "int") sz += 4;
        else if (iter->second.m_type == "float") sz += 4;
        else if (iter->second.m_type == "double") sz += 8;
    }
    LOG_INFO << "Row data size: " << sz;
}

} // namespace lincore

