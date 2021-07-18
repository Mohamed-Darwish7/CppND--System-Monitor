#include <string>

#include "format.h"

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
  int TimeHr {0}, TimeMin {0}, TimeSec {0};
  
  TimeHr = seconds / 3600;
  TimeMin = (seconds % 3600) / 60;
  TimeSec = (seconds % 3600) % 60;
  
  string Time = to_string(TimeHr) + " : " + to_string(TimeMin) + " : "  + to_string(TimeSec);
  
  return Time; 
}
