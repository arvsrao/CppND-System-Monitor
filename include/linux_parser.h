#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <map>
#include <utility>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/proc/uptime"};
const std::string kMeminfoFilename{"/proc/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

// keys to parse
const std::string processesKey("processes");
const std::string runningProcessesKey("procs_running");
const std::string memTotalKey("MemTotal:");
const std::string memFreeKey("MemFree:");
const std::string cpuKey("cpu");
const std::string uidKey("Uid:");
const std::string physicalMemUsedKey("VmRSS:");

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int ParseIntFromStatsFile(std::string const& matchKey);
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long TotalTime(int pid);

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
long int UpTime(int pid);
std::map<std::string, std::string> buildMap();
};  // namespace LinuxParser

#endif