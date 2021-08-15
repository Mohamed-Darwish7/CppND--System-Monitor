#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  std::vector<std::string> Jiffies = LinuxParser::CpuUtilization();
  int count {0};
  
  for(auto Jiffie : Jiffies){
  	if(count != 3 || count != 4){
    	Nonidle += std::stof(Jiffie);
    }
    if(count == 3 || count == 4){
    	idle += std::stof(Jiffie);
    }
    count ++;
  }
  
  bool Current(true);
  if(Current){
    total = Nonidle + idle;
  	totald = total - Prevtotal;
    idled = idle - Previdle;
    CpuPercentage = (totald - idled) / totald;
    
    Previdle = idle;
    PrevNonidle = Nonidle;
    Prevtotal = total;  
  }
  
  return CpuPercentage;
}
