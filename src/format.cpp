#include <string>
#include <vector>
#include <algorithm>
#include "format.h"

using std::string;
using std::vector;

/**
 * INPUT:  Long int measuring seconds
 * OUTPUT: HH:MM:SS
 */
string Format::ElapsedTime(long seconds) {
  vector<long> units  = { seconds / 3600, seconds / 60, seconds % 60 };
  vector<string> digits(units.size());
  std::transform(units.begin(), units.end(), digits.begin(),[](long& digit) {
    return digit < 10 ? "0" + std::to_string(digit) : std::to_string(digit);
  });

  // accumulate the time string
  return digits[0] + ":" + digits[1] + ":" + digits[2];
}