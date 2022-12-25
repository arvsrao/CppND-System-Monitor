#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

/** constant UID to Username lookup map */
const std::map<std::string, std::string> Process::uidToUsernameMap = LinuxParser::buildMap();

/** The sole constructor */
Process::Process(int pid) : pid_(pid),
                            start_time_(LinuxParser::UpTime(pid)),
                            user_(uidToUsernameMap.at(LinuxParser::Uid(pid))),
                            command_(LinuxParser::Command(pid)) {}

/** Forbid use of the default constructor; the default constructor is set private */
Process::Process() {}

/** Return this process's ID */
int Process::Pid() const { return pid_; }

/** Return this process's CPU utilization as a decimal fraction. */
float Process::CpuUtilization() const {
  return (float) LinuxParser::TotalTime(Pid()) / (float) UpTime();
}

/** Return the command that generated this process */
string Process::Command() { return command_; }

/** Return this process's memory utilization */
string Process::Ram() const {
  auto spaceInMBs = std::stoi(LinuxParser::Ram(Pid())) / 1024;
  return std::to_string(spaceInMBs);
}

/** Return the user (name) that generated this process */
string Process::User() const { return user_; }

/** Return the age of this process (in seconds) */
long int Process::UpTime() const { return LinuxParser::UpTime() - start_time_; }

/** Less than comparison operator for Process objects */
bool Process::operator<(Process const& rhs) const { return CpuUtilization() < rhs.CpuUtilization(); }

/** Greater than comparison operator for Process objects */
bool Process::operator>(Process const& rhs) const { return rhs < *this; }
