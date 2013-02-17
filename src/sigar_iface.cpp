/**********************************************
   File:   sigar_iface.cpp

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

#include "sigar_iface.h"
#include "sigar/sigar.h"
#include "sigar/sigar_format.h"
#include <sys/statvfs.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <string.h>

namespace lincore {
    
static const int SECTOR_SIZE = 512;


#define SIGAR_DECL(s) \
    if (m_handle == NULL) { \
        fprintf(stderr, "Failed to execute unitialized library call\n"); \
        throw -1; \
    } \
    sigar_t* s = (sigar_t*) m_handle;

#define SIGAR_GET(DataType,SigarFunction,DataName) \
    DataType data; \
    int res = SigarFunction(sigar, &data); \
    if (res != SIGAR_OK) { \
        fprintf(stderr, "Failed to get %s\n", DataName); \
        throw -1; \
    }

#define SIGAR_GET_BY_PID(DataType,SigarFunction,DataName) \
    DataType data; \
    int res = SigarFunction(sigar, pid, &data); \
    if (res != SIGAR_OK) { \
        fprintf(stderr, "Failed to get %s\n", DataName); \
        throw -1; \
    }

#define SIGAR_GET_BY_NAME(DataType,SigarFunction,DataName) \
    DataType data; \
    int res = SigarFunction(sigar, name, &data); \
    if (res != SIGAR_OK) { \
        fprintf(stderr, "Failed to get %s\n", DataName); \
        throw -1; \
    }



void SigarIface::init()
{
    if (m_handle != NULL) return;

    sigar_t* sigar;
    int res = sigar_open(&sigar);
    if (res != SIGAR_OK) {
        fprintf(stderr, "Failed to open sigar\n");
        throw -1;
    }

    m_handle = sigar;
}

void SigarIface::uninit()
{
    if (m_handle == NULL) return;

    sigar_t* sigar = (sigar_t*) m_handle;
    int res = sigar_close(sigar);
    if (res != SIGAR_OK) {
        fprintf(stderr, "Failed to close sigar\n");
    }

    m_handle = NULL;
}

void SigarIface::getLoadAverages(LoadAverages& systemLoad)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_loadavg_t,sigar_loadavg_get,"load averages");
    systemLoad._1min = data.loadavg[0];
    systemLoad._5min = data.loadavg[1];
    systemLoad._15min = data.loadavg[2];
}

void SigarIface::getMemory(Memory& memory)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_mem_t,sigar_mem_get,"memory");
    memory.total = data.total;
    memory.used = data.used;
    memory.free = data.free;
    memory.actualUsed = data.actual_used;
    memory.actualFree = data.actual_free;
    memory.percentUsed = data.used_percent;
    memory.percentFree = data.free_percent;

    memory.total /= 1024;
    memory.used /= 1024;
    memory.free /= 1024;
    memory.actualUsed /= 1024;
    memory.actualFree /= 1024;
}

void SigarIface::getSwap(Swap& swap)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_swap_t,sigar_swap_get,"swap");
    swap.total = data.total / 1024;
    swap.used = data.used / 1024;
    swap.free = data.free / 1024;
    swap.page_in = data.page_in;
    swap.page_out = data.page_out;
}

void SigarIface::getSwapMetricsDiff(const Swap& prev, const Swap& curr, Swap& nm)
{
#define NM_DIFF(f)  nm.f = (curr.f <= prev.f) ? 0 : curr.f - prev.f;
    NM_DIFF(page_in);
    NM_DIFF(page_out);
#undef NM_DIFF

    nm.total = curr.total;
    nm.used = curr.used;
    nm.free = curr.free;
}

void SigarIface::getCPU(CPU& cpu)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_cpu_t,sigar_cpu_get,"cpu");
    cpu.user = data.user;
    cpu.sys = data.sys;
    cpu.nice = data.nice;
    cpu.idle = data.idle;
    cpu.wait = data.wait;
    cpu.irq = data.irq;
    cpu.softIrq = data.soft_irq;
    cpu.stolen = data.stolen;
    cpu.total = data.total;
}

void SigarIface::getCPUPercent(const CPU& prev, const CPU& curr, CPUPercent& perc)
{
    double diff_user, diff_sys, diff_nice, diff_idle;
    double diff_wait, diff_irq, diff_soft_irq, diff_stolen;
    double diff_total;

    diff_user = curr.user - prev.user;
    diff_sys  = curr.sys  - prev.sys;
    diff_nice = curr.nice - prev.nice;
    diff_idle = curr.idle - prev.idle;
    diff_wait = curr.wait - prev.wait;
    diff_irq = curr.irq - prev.irq;
    diff_soft_irq = curr.softIrq - prev.softIrq;
    diff_stolen = curr.stolen - prev.stolen;

    diff_user = diff_user < 0 ? 0 : diff_user;
    diff_sys  = diff_sys  < 0 ? 0 : diff_sys;
    diff_nice = diff_nice < 0 ? 0 : diff_nice;
    diff_idle = diff_idle < 0 ? 0 : diff_idle;
    diff_wait = diff_wait < 0 ? 0 : diff_wait;
    diff_irq = diff_irq < 0 ? 0 : diff_irq;
    diff_soft_irq = diff_soft_irq < 0 ? 0 : diff_soft_irq;
    diff_stolen = diff_stolen < 0 ? 0 : diff_stolen;

    diff_total =
        diff_user + diff_sys + diff_nice + diff_idle +
        diff_wait + diff_irq + diff_soft_irq +
        diff_stolen;

    perc.user = diff_user / diff_total * 100;
    perc.sys  = diff_sys / diff_total * 100;
    perc.nice = diff_nice / diff_total * 100;
    perc.idle = diff_idle / diff_total * 100;
    perc.wait = diff_wait / diff_total * 100;
    perc.irq = diff_irq / diff_total * 100;
    perc.softIrq = diff_soft_irq / diff_total * 100;
    perc.stolen = diff_stolen / diff_total * 100;

    perc.combined =
        perc.user + perc.sys + perc.nice + perc.wait;

}

void SigarIface::getProcessCount(ProcessCount& processCount)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_proc_stat_t,sigar_proc_stat_get,"process count");
    processCount.total = data.total;
    processCount.threads = data.threads;
}

static void matchProcesses(sigar_t* sigar, 
                            ProcessFilters& filters, 
                            sigar_proc_list_t& data, 
                            ProcessIDs& procs)
{
    int res;
    sigar_proc_state_t procState;
    sigar_proc_args_t procArgs;
    ProcessFilters::iterator iter;

    for (unsigned long i=0; (i < data.number) && (procs.size() < filters.size()); i++) {
        res = sigar_proc_state_get(sigar, data.data[i], &procState);
        if (res != SIGAR_OK) continue;

        for (iter=filters.begin(); iter != filters.end(); iter++) {
            if (strcmp(procState.name, iter->exec.c_str()) != 0) continue;

            res = sigar_proc_args_get(sigar, data.data[i], &procArgs);
            if (res != SIGAR_OK) continue;

            for (unsigned long j=0; j < procArgs.number; j++) {
                if (strstr(procArgs.data[j], iter->args.c_str()) == 0) continue;
                
                ProcessID processID = { iter->id, data.data[i] };
                procs.push_back(processID);
                break;
            }

            sigar_proc_args_destroy(sigar, &procArgs);
        }
    }
}

void SigarIface::getProcessIDs(ProcessFilters& filters, ProcessIDs& procs)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_proc_list_t,sigar_proc_list_get,"process list");
    try {
        matchProcesses(sigar, filters, data, procs);
    }
    catch (...) {
        sigar_proc_list_destroy(sigar, &data);
        throw;
    }
    sigar_proc_list_destroy(sigar, &data);
}

void SigarIface::getProcessTimes(int pid, ProcessTimes& pt)
{
    SIGAR_DECL(sigar);
    SIGAR_GET_BY_PID(sigar_proc_time_t,sigar_proc_time_get,"process time");
    pt.startTime = data.start_time;
    pt.user = data.user;
    pt.sys = data.sys;
    pt.total = data.total;
}

void SigarIface::getProcessMetrics(int pid, const ProcessTimes& pt, int lastTime, ProcessMetrics& pm)
{
    SIGAR_DECL(sigar);
    {
        sigar_proc_cpu_t data;
        data.start_time = pt.startTime;
        data.user = pt.user;
        data.sys = pt.sys;
        data.total = pt.total;
        data.last_time = lastTime;
        int res = sigar_proc_cpu_get(sigar, pid, &data);
        if (res != SIGAR_OK) {
            fprintf(stderr, "Failed to get %s\n", "process cpu");
            throw -1;
        }
        pm.cpu = data.percent;
    }
    {
        SIGAR_GET_BY_PID(sigar_proc_mem_t,sigar_proc_mem_get,"process memory");
        pm.memory = data.size;
    }
}

void SigarIface::getFileSystems(int type, FileSystems& fs)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_file_system_list_t,sigar_file_system_list_get,"file system list");
    for (unsigned int i=0; i < data.number; i++) {
        if ((type >= 0) && (data.data[i].type != type)) continue;
        FileSystem f;
        f.dirName = data.data[i].dir_name;
        f.devName = data.data[i].dev_name;
        f.typeName = data.data[i].type_name;
        f.sysTypeName = data.data[i].sys_type_name;
        f.type = data.data[i].type;
        fs.push_back(f);
    }
    sigar_file_system_list_destroy(sigar, &data);
}

void SigarIface::getDisk(const std::string& diskName, Disk& disk)
{
    /********************************************
    SIGAR_DECL(sigar);
    const char* name = diskName.c_str();
    SIGAR_GET_BY_NAME(sigar_disk_usage_t,sigar_disk_usage_get,"disk usage");

    disk.reads = data.reads;
    disk.writes = data.writes;
    disk.writeBytes = data.write_bytes;
    disk.readBytes = data.read_bytes;
    disk.readTime = data.rtime;
    disk.writeTime = data.wtime;
    disk.waitTime = data.wtime;
    disk.queueTime = data.qtime;
    disk.totalTime = data.time;
    disk.snapTime = data.snaptime;
    disk.serviceTime = data.service_time;
    disk.queue = data.queue;
     *********************************************/

    size_t n = 0;
    for ( ; n < m_diskStats.size(); n++) {
        if (diskName == m_diskStats[n].m_name) break;
    }
    if (n == m_diskStats.size()) return;

    disk.reads = m_diskStats[n].m_reads;
    disk.writes = m_diskStats[n].m_writes;
    disk.readBytes = m_diskStats[n].m_sectorsRead * SECTOR_SIZE;
    disk.writeBytes = m_diskStats[n].m_sectorsWritten * SECTOR_SIZE;
    disk.readTime = m_diskStats[n].m_millisecRead;
    disk.writeTime = m_diskStats[n].m_millisecWrite;
    disk.totalTime = m_diskStats[n].m_millisecIO;
    disk.waitTime = disk.totalTime - (disk.readTime + disk.writeTime);
    disk.queue = m_diskStats[n].m_queue;
}

void SigarIface::getDisk(Disk& disk)
{
    for (size_t n = 0; n < m_diskStats.size(); n++) {
        disk.reads += m_diskStats[n].m_reads;
        disk.writes += m_diskStats[n].m_writes;
        disk.readBytes += m_diskStats[n].m_sectorsRead * SECTOR_SIZE;
        disk.writeBytes += m_diskStats[n].m_sectorsWritten * SECTOR_SIZE;
        disk.readTime += m_diskStats[n].m_millisecRead;
        disk.writeTime += m_diskStats[n].m_millisecWrite;
        disk.totalTime += m_diskStats[n].m_millisecIO;
        disk.queue += m_diskStats[n].m_queue;
    }
    disk.waitTime = disk.totalTime - (disk.readTime + disk.writeTime);
}

void SigarIface::readDisksStats()
{
    m_diskStats.clear();

    char buf[512];
    FILE* f = fopen("/proc/diskstats", "r");
    while (!feof(f)) {
        char* s = fgets(buf, sizeof(buf), f);
        if (s == NULL) break;

        for ( ; *s; s++) {
            if (!((*s == ' ') || ((*s >= '0') && (*s <= '9')))) break;
        }
        if (*s == '\0') {
            fprintf(stderr, "Failed to read diststats: disk name not found\n");
            throw -1;
        }
    
        char* t = s;
        for ( ; *t && (*t != ' '); t++);
        if (*t == '\0') {
            fprintf(stderr, "Failed to read diststats: values not found\n");
            throw -1;
        }

        *t = '\0';
        t++;

        if (strstr(s, "ram") || strstr(s, "loop")) continue;

        DiskStats ds;
        int ret = sscanf(t, "%lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
                  &ds.m_reads ,
                  &ds.m_readsMerged ,
                  &ds.m_sectorsRead ,
                  &ds.m_millisecRead ,
                  &ds.m_writes ,
                  &ds.m_writesMerged ,
                  &ds.m_sectorsWritten ,
                  &ds.m_millisecWrite ,
                  &ds.m_queue ,
                  &ds.m_millisecIO ,
                  &ds.m_weightedTime);
        if (ret != 11) {
            fprintf(stderr, "Failed to read diststats: values not parsed\n");
            throw -1;
        }

        strncpy(ds.m_name, s, sizeof(ds.m_name));

        m_diskStats.push_back(ds);
    }

}

void SigarIface::getDiskMetricsDiff(const Disk& prev, const Disk& curr, Disk& nm)
{
#define NM_DIFF(f)  nm.f = (curr.f <= prev.f) ? 0 : curr.f - prev.f;
    NM_DIFF(reads);
    NM_DIFF(writes);
    NM_DIFF(readBytes);
    NM_DIFF(writeBytes);
    NM_DIFF(readTime);
    NM_DIFF(writeTime);
    NM_DIFF(waitTime);
    NM_DIFF(queueTime);
    NM_DIFF(totalTime);
    NM_DIFF(snapTime);
    NM_DIFF(serviceTime);
#undef NM_DIFF

    nm.queue = curr.queue;
}

void SigarIface::getFS(const std::string& dirName, FSInfo& fsInfo)
{
    /******************************************
    SIGAR_DECL(sigar);
    const char* name = dirName.c_str();
    SIGAR_GET_BY_NAME(sigar_file_system_usage_t,sigar_file_system_usage_get,"file system usage");

    fsInfo.usedPercent = data.use_percent;
    fsInfo.totalSpace = data.total;
    fsInfo.freeSpace = data.free;
    fsInfo.usedSpace = data.used;
    fsInfo.availSpace = data.avail;
    ******************************************/

    struct statvfs fs;
    int ret = statvfs(dirName.c_str(), &fs);
    if (ret != 0) {
        fprintf(stderr, "Failed to get file system info for %s (%s)\n", dirName.c_str(), strerror(errno));
        throw -1;
    }

    fsInfo.totalSpace = fs.f_frsize * fs.f_blocks;
    fsInfo.freeSpace = fs.f_bfree * fs.f_bsize;
    fsInfo.availSpace = fs.f_bavail * fs.f_bsize;
    fsInfo.usedSpace = fsInfo.totalSpace - fsInfo.freeSpace;
    fsInfo.usedPercent = fsInfo.usedSpace / fsInfo.totalSpace * 100;

    double div = 1024;
    fsInfo.totalSpace /= div;
    fsInfo.freeSpace /= div;
    fsInfo.availSpace /= div;
    fsInfo.usedSpace = fsInfo.totalSpace - fsInfo.freeSpace; // recalc in KB
}

void SigarIface::getNetInfo(NetInfo& netInfo)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_net_info_t,sigar_net_info_get,"net info");
    netInfo.hostName = data.host_name;
    netInfo.domainName = data.domain_name;
}

void SigarIface::getNet(const std::string& netName, NetMetrics& nm)
{
    SIGAR_DECL(sigar);
    const char* name = netName.c_str();
    SIGAR_GET_BY_NAME(sigar_net_interface_stat_t,
                      sigar_net_interface_stat_get,
                      "net interace stat");
    nm.rxPackets = data.rx_packets;
    nm.rxBytes = data.rx_bytes;
    nm.rxErrors = data.rx_errors;
    nm.rxDropped = data.rx_dropped;
    nm.rxOverruns = data.rx_overruns;
    nm.txPackets = data.tx_packets;
    nm.txBytes = data.tx_bytes;
    nm.txErrors = data.tx_errors;
    nm.txDropped = data.tx_dropped;
    nm.txOverruns = data.tx_overruns;
}

void SigarIface::getNetMetricsDiff(const NetMetrics& prev, const NetMetrics& curr, NetMetrics& nm)
{
#define NM_DIFF(f)  nm.f = (curr.f <= prev.f) ? 0 : curr.f - prev.f;
    NM_DIFF(rxPackets);
    NM_DIFF(rxBytes);
    NM_DIFF(rxErrors);
    NM_DIFF(rxDropped);
    NM_DIFF(rxOverruns);

    NM_DIFF(txPackets);
    NM_DIFF(txBytes);
    NM_DIFF(txErrors);
    NM_DIFF(txDropped);
    NM_DIFF(txOverruns);
#undef NM_DIFF
}

void SigarIface::getCpuCores(int& cores)
{
    cores = 0;
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_cpu_info_list_t,sigar_cpu_info_list_get,"cpu info list");
    for (unsigned int i=0; i < data.number; i++) {
        cores += data.data[i].total_cores;
    }
    sigar_cpu_info_list_destroy(sigar, &data);
}

void SigarIface::getTcp(Tcp& tcp)
{
    SIGAR_DECL(sigar);
    SIGAR_GET(sigar_tcp_t,sigar_tcp_get,"tcp");
    tcp.connOpens = data.active_opens + data.passive_opens;
    tcp.connFailed = data.attempt_fails;
    tcp.connections = data.curr_estab;
    tcp.inSeg = data.in_segs;
    tcp.outSeg = data.out_segs;
    tcp.retransmits = data.retrans_segs;
}

} // namespace lincore

