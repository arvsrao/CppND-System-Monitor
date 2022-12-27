#include "system.h"

#include <algorithm>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::size_t;
using std::string;
using std::vector;

/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/

System::System()
    : cpu_(Processor()),
      kernel_(LinuxParser::Kernel()),
      operating_system_(LinuxParser::OperatingSystem()) {}

/** Return an instance of Processor */
Processor& System::Cpu() { return cpu_; }

/** Return a container composed of the system's processes */
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();

  if (processes_.empty()) {
    std::transform(pids.begin(), pids.end(), std::back_inserter(processes_),
                   [](int pid) { return Process(pid); });
  } else {
    std::sort(processes_.begin(), processes_.end(),
              [](Process& a, Process& b) { return a.Pid() < b.Pid(); });
    vector<Process> temp;
    size_t pidIdx = 0, processIdx = 0;

    //  processes: 7  && pids: 55
    while (pidIdx < pids.size() ) {
      if (processIdx >= processes_.size() || processes_[processIdx].Pid() > pids[pidIdx]) {
        temp.emplace_back(Process(pids[pidIdx]));
        pidIdx++;
      }
      else if (processes_[processIdx].Pid() < pids[pidIdx]) processIdx++;  // remove processes_[processIdx]
      else {  // keep pid && advance both pointers
          temp.emplace_back(processes_[pidIdx]);
          pidIdx++;
          processIdx++;
        }
    }
    processes_.clear();
    processes_ = temp;
  }

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
