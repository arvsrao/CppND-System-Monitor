#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include <map>

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& rhs) const;
  bool operator>(Process const& rhs) const;

  // constructor
  explicit Process(int pid);

 private:

  // forbid use of default constructor
  Process();

  // truncate a command if necessary
  static std::string truncateCommand(const std::string& cmd);

  static const std::map<std::string, std::string> uidToUsernameMap;
  int pid_;
  long int start_time_;
  std::string user_{}, command_{};
};

#endif