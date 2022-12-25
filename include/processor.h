#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
class Processor {
 public:

  Processor();
  float Utilization();

 private:
  float prevIdle_, prevTotal_;

  std::vector<float> ComputeIdleAndTotal();

};

#endif