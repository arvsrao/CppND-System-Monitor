#include "processor.h"

#include <vector>
#include "linux_parser.h"

using LinuxParser::CPUStates;

/** Constructor. Initialize the prevIdle and prevTotal. */
Processor::Processor() {
    auto initPrev = ComputeIdleAndTotal();
    prevIdle_  = initPrev[0];
    prevTotal_ = initPrev[1];
}

/**
 * Extract the Idle and Total times needed to compute the Utilization according
 * to the formula in StackOverFlow answer https://stackoverflow.com/a/23376195
 * */
std::vector<float> Processor::ComputeIdleAndTotal() {

  std::vector<float> cpuTimes;
  auto extractedTimes = LinuxParser::CpuUtilization();

  std::transform(extractedTimes.begin(),
                 extractedTimes.end(),
                 std::back_inserter(cpuTimes),
                 [](const std::string& cpuTime) { return (float) std::stoi(cpuTime); });

  auto Idle    = cpuTimes[CPUStates::kIdle_] + cpuTimes[CPUStates::kIOwait_];
  auto NonIdle = cpuTimes[CPUStates::kUser_] + cpuTimes[CPUStates::kNice_]
      + cpuTimes[CPUStates::kSystem_] + cpuTimes[CPUStates::kIRQ_]
      + cpuTimes[CPUStates::kSoftIRQ_] + cpuTimes[CPUStates::kSteal_];

  return {Idle, Idle + NonIdle};
}

/** Return the aggregate CPU utilization.
 *
 * Formula for Utilization taken from https://stackoverflow.com/a/23376195
 * */
float Processor::Utilization() {

  auto curr = ComputeIdleAndTotal();
  float idled  = curr[0] - prevIdle_;
  float totald = curr[1] - prevTotal_;

  prevIdle_  = curr[0];
  prevTotal_ = curr[1];

  // divid-by-zero guard
  return (totald > 0) ? (totald - idled) / totald : 0.f;
}

