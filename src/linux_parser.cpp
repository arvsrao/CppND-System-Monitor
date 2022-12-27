#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <experimental/filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::map;
using std::to_string;
using std::vector;
using UidUserPair = std::pair<std::string, std::string>;
namespace fs = std::experimental::filesystem;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

/** Extract all the PIDs from the folder names in '/proc/'. */
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  const fs::path procDirectory(kProcDirectory);

  // loop over directory contents
  for (auto const& entry : fs::directory_iterator{kProcDirectory}) {
    if (fs::is_directory(entry.status())) {
      auto dirName = entry.path().stem().string();
      if (std::all_of(dirName.begin(), dirName.end(), isdigit)) {
        int pid = stoi(dirName);
        pids.push_back(pid);
      }
    }
  }

  std::sort(pids.begin(), pids.end(), [](int a, int b) { return a < b;});
  return pids;
}

/** Read and return the system memory utilization */
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float memTotal{1.f}, memFree{0.f};
  std::ifstream filestream(kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") memTotal = (float) stoi(value);
        if (key == "MemFree:") memFree   = (float) stoi(value);
      }
    }
  }
  return memFree / memTotal;
}

/** Read and return the system uptime (in seconds) */
long LinuxParser::UpTime() {
  string line, uptime, idle;
  std::ifstream filestream(kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> uptime >> idle) {
        return stol(uptime);
      }
    }
  }
  return 0;
}

/** Read and parse CPU utilization from '/proc/stat' */
vector<string> LinuxParser::CpuUtilization() {
  string line, key, user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  vector<string> retVal;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice) {
        if (key == "cpu") {
          return { user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice };
        }
      }
    }
  }
  return {};
}

/** Read and return Total Time for a process from '/proc/${pid}/stat' */
long LinuxParser::TotalTime(int pid) {
  string line;
  long total_time{0};
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    int idx = 1;
    while (std::getline(filestream, line, ' ')) {
      if (idx > 13 && idx < 18) total_time += stol(line);
      if ( idx > 18) break;
      idx++;
    }
    total_time /= sysconf(_SC_CLK_TCK);
    return total_time;
  }
  return 0;
}

/** Read and return the a number from the 'proc/stat' file.
 * @param matchKey identifies the row in 'proc/stat' to parse
 * @return -1 if there is NO key match or value is not an integer
 * */
int LinuxParser::GetNumberOfProcesses(string const& matchKey) {
  string line, key, total;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> total) {
        if (key == matchKey && std::all_of(total.begin(), total.end(), isdigit)) {
          return std::stoi(total);
        }
      }
    }
  }
  return -1;
}

/** Read and return the total number of processes */
int LinuxParser::TotalProcesses() {
  return LinuxParser::GetNumberOfProcesses("processes");
}

/** Read and return the number of running processes */
int LinuxParser::RunningProcesses() {
  return LinuxParser::GetNumberOfProcesses("procs_running");
}

/** Read and return the command associated with a process */
string LinuxParser::Command(int pid) {
  string line, cmd;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

/** Read and return the memory (in KBs) used by a process */
string LinuxParser::Ram(int pid) {
  string line, key, vmSize;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> vmSize) {
        if (key == "VmSize:") return vmSize;
      }
    }
  }
  return {};
}

/** Read and return the user ID associated with a process */
string LinuxParser::Uid(int pid) {
    string line, key, uid;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> uid) {
          if (key == "Uid:") return uid;
        }
      }
    }
    return "";
}

///** Read and return the user associated with a process */
//string LinuxParser::User(int pid) {
//    string uid = LinuxParser::Uid(pid);
//}

std::map<string, string> LinuxParser::buildMap() {
  std::map<string, string> uidToUsernameMap;
  string line, username, uid, x ;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      if (linestream >> username >> x >> uid) {
        uidToUsernameMap.insert(UidUserPair(uid, username));
      }
    }
  }
  return uidToUsernameMap;
}

/** Read and return the start time of a process (in seconds) */
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);

  if (filestream.is_open()) {
    int idx = 1;
    while (std::getline(filestream, line, ' ') && idx < 22) { idx++; }
    return stol(line) / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
