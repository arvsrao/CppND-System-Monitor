#include "system.h"

#include <algorithm>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::string;
using std::vector;

/*
 * You need to complete the mentioned TODOs in order to satisfy the rubric criteria
 *    "The student will be able to extract and display basic data about the system."
 *
 * You need to properly format the uptime. Refer to the comments mentioned in
 * format.cpp for formatting the uptime.
 * */

System::System()
    : cpu_(Processor()),
      kernel_(LinuxParser::Kernel()),
      operating_system_(LinuxParser::OperatingSystem()) {}

/** Return an instance of Processor */
Processor& System::Cpu() { return cpu_; }

/** Return a container composed of the system's processes */
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  processes_.clear();
  std::transform(pids.begin(), pids.end(), std::back_inserter(processes_),
                 [](int pid) { return Process(pid); });

  // sort Processes in decreasing order of CPU %
  std::sort(processes_.begin(), processes_.end(),[](Process& a, Process& b) { return a > b; });
  return processes_;
}

/** Return the system's kernel identifier (string) */
std::string System::Kernel() { return kernel_; }

/** Return the system's memory utilization */
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

/** Return the operating system name */
std::string System::OperatingSystem() { return operating_system_; }

/** Return the number of processes actively running on the system */
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

/** Return the total number of processes on the system */
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

/** Return the number of seconds since the system started running */
long int System::UpTime() { return LinuxParser::UpTime(); }
