/**********************************************
   File:   sigar_iface.h

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

#ifndef _SIGAR_IFACE_H_
#define _SIGAR_IFACE_H_

#include <vector>
#include <list>
#include <string>

/************************************
 * Define wrapper above Hyperic SIGAR interfaces
 ************************************/
namespace lincore {

struct LoadAverages
{
    LoadAverages() : _1min(0.0), _5min(0.0), _15min(0) {}
    double _1min;
    double _5min;
    double _15min;
};

struct Memory
{
    Memory() : total(0), used(0), free(0), actualUsed(0), 
               actualFree(0), percentUsed(0.0), percentFree(0.0) {}
    double total;
    double used;
    double free;
    double actualUsed;
    double actualFree;
    double percentUsed;
    double percentFree;
};

struct Swap
{
    Swap() : total(0), used(0), free(0), page_in(0), page_out(0) {}
    double total;
    double used;
    double free;
    double page_in;
    double page_out;
};

struct CPU
{
    CPU() : user(0), sys(0), nice(0), idle(0), wait(0), irq(0), softIrq(0),
            stolen(0), total(0) {}
    double user;
    double sys;
    double nice;
    double idle;
    double wait;
    double irq;
    double softIrq;
    double stolen;
    double total;
};

struct CPUPercent
{
    CPUPercent() : user(0.0), sys(0.0), nice(0.0), idle(0.0), wait(0.0), irq(0.0), 
                  softIrq(0.0), stolen(0.0), combined(0.0) {}
    double user;
    double sys;
    double nice;
    double idle;
    double wait;
    double irq;
    double softIrq;
    double stolen;
    double combined;
};

struct ProcessCount
{
    ProcessCount() : total(0), threads(0) {}
    double total;
    double threads;
};

struct ProcessFilter
{
    int id;
    std::string exec;
    std::string args;
};
typedef std::list<ProcessFilter> ProcessFilters;

struct ProcessID
{
    int id;
    int pid;
};
typedef std::vector<ProcessID> ProcessIDs;

struct ProcessTimes
{
    ProcessTimes() : startTime(0), user(0), sys(0), total(0) {}
    double startTime;
    double user;
    double sys;
    double total;
};

struct ProcessMetrics
{
    double memory;
    double        cpu;
};

struct FileSystem
{
    static const int LOCAL_FILE_SYSTEM = 2;
    std::string dirName;
    std::string devName;
    std::string typeName;
    std::string sysTypeName;
    int         type;
};
typedef std::vector<FileSystem> FileSystems;

struct DiskStats
{
    char m_name[64];
    unsigned long m_reads;               // 1
    unsigned long m_readsMerged;         // 2
    unsigned long m_sectorsRead;         // 3
    unsigned long m_millisecRead;        // 4
    unsigned long m_writes;              // 5
    unsigned long m_writesMerged;        // 6
    unsigned long m_sectorsWritten;      // 7
    unsigned long m_millisecWrite;       // 8
    unsigned long m_queue;               // 9
    unsigned long m_millisecIO;          // 10
    unsigned long m_weightedTime;        // 11
};

struct Disk
{
    Disk() : reads(0), writes(0), writeBytes(0), readBytes(0), readTime(0),
             writeTime(0), waitTime(0), queueTime(0), totalTime(0),
             snapTime(0), serviceTime(0), queue(0) {}
    double reads;
    double writes;
    double writeBytes;
    double readBytes;
    double readTime;
    double writeTime;
    double waitTime;
    double queueTime;
    double totalTime;
    double snapTime;
    double serviceTime;
    double queue;
};

struct FSInfo
{
    FSInfo() : usedPercent(0),
               totalSpace(0), freeSpace(0), usedSpace(0), availSpace(0) {}
    double usedPercent;
    double totalSpace;
    double freeSpace;
    double usedSpace;
    double availSpace;
};

struct NetInfo
{
    std::string hostName;
    std::string domainName;
};

struct NetMetrics
{
    NetMetrics() : rxPackets(0), rxBytes(0), rxErrors(0), rxDropped(0), rxOverruns(0),
                   txPackets(0), txBytes(0), txErrors(0), txDropped(0), txOverruns(0) {}
    double rxPackets;
    double rxBytes;
    double rxErrors;
    double rxDropped;
    double rxOverruns;

    double txPackets;
    double txBytes;
    double txErrors;
    double txDropped;
    double txOverruns;
};

struct Tcp
{
    Tcp() : connOpens(0), connFailed(0), connections(0), inSeg(0), outSeg(0), 
            retransmits(0) {}
    double connOpens;
    double connFailed;
    double connections;
    double inSeg;
    double outSeg;
    double retransmits;
};

class SigarIface
{
public:
    SigarIface() :  m_handle(0) {}
    void init();
    void uninit();

    void getLoadAverages(LoadAverages& systemLoad);
    void getMemory(Memory& memory);
    void getSwap(Swap& swap);
    void getSwapMetricsDiff(const Swap& prev, const Swap& curr, Swap& nm);
    void getCPU(CPU& cpu);
    void getCPUPercent(const CPU& prev, const CPU& curr, CPUPercent& cpuPerc);
    void getProcessCount(ProcessCount& processCount);
    void getProcessIDs(ProcessFilters& filters, ProcessIDs& procs);
    void getProcessTimes(int pid, ProcessTimes& processTimes);
    void getProcessMetrics(int pid, const ProcessTimes& pt, int lastTime, ProcessMetrics& pm);
    void getFileSystems(int type, FileSystems& fs);
    void readDisksStats();
    void getDisk(const std::string& diskName, Disk& disk);
    void getDisk(Disk& disk);
    void getDiskMetricsDiff(const Disk& prev, const Disk& curr, Disk& nm);
    void getFS(const std::string& dirName, FSInfo& fsInfo);
    void getNetInfo(NetInfo& netInfo);
    void getNet(const std::string& netName, NetMetrics& nm);
    void getNetMetricsDiff(const NetMetrics& prev, const NetMetrics& curr, NetMetrics& nm);
    void getCpuCores(int& cores);
    void getTcp(Tcp& tcp);

private:
    std::vector< DiskStats > m_diskStats;

private:
    void* m_handle;

};

} // namespace lincore


#endif // _SIGAR_IFACE_H_

