#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <format.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::stol;
using std::getline;

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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string MemoryName {}, MemoryValue {};
  float TotalUsedMemory {0};
  
  std::ifstream myMemory(kProcDirectory + kMeminfoFilename);

  if(myMemory.is_open()){
  	string line;
    while(getline(myMemory, line)){
    	std::istringstream myStream(line);
      	while(myStream >> MemoryName >> MemoryValue){
          	if(MemoryName == "MemTotal:")
              	TotalUsedMemory = stof(MemoryValue);
          	else if(MemoryName == "MemFree:")
              	TotalUsedMemory = TotalUsedMemory - stof(MemoryValue);
          else if(MemoryName == "Buffers:")
            	TotalUsedMemory = TotalUsedMemory - stof(MemoryValue);
          else if(MemoryName == "Cashed:")
            	TotalUsedMemory = TotalUsedMemory - stof(MemoryValue);
          else break;
        }
    }
  }
  std::cout << TotalUsedMemory << std::endl;
  return TotalUsedMemory;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line, Uptime;
  long UpTimeInt;
  
  std::ifstream UpTime(kProcDirectory + kUptimeFilename);
  if(UpTime.is_open()){
    getline(UpTime, line);
    std::istringstream myStream(line);
	myStream >> Uptime;
  }
  UpTimeInt = stol(Uptime);
  
  return UpTimeInt; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long TimeInSeconds = UpTime();
  long TimeInJiffies = TimeInSeconds * sysconf(_SC_CLK_TCK);
  return TimeInJiffies; 
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line, variable;
  long TotalTimeInJiffies {0};
  int count {0};
  std::ifstream myActiveJiffies(kProcDirectory + to_string(pid) + kStatFilename);
  
  if(myActiveJiffies.is_open()){
  	getline(myActiveJiffies, line);
    std::istringstream myStream(line);
    while(myStream >> variable){
      if(count == 13 || count == 14 || count == 15 || count == 16 || count == 21){
      	TotalTimeInJiffies += stol(variable);
      }
      count++;
    }
  }
  
  return TotalTimeInJiffies; 
}                                                         

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  string line, variable;
  long TimeInJiffies {}, TimeInSeconds {};
  int count{0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if(stream.is_open()){
  	getline(stream,line);
    std::istringstream myStream(line);
    while(myStream >> variable){
      if(count == 0)
        continue;
      else if(count == 1||count == 2||count == 3||count == 6 || count == 7 || count == 8){
        TimeInJiffies += stol(variable);
      }
      count ++;
    }
  }
  TimeInSeconds = TimeInJiffies / sysconf(_SC_CLK_TCK);
  return TimeInSeconds; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, variable;
  long IdleTimeInJiffies {};
  int count {0};
  std::ifstream stream(kProcDirectory + kStatFilename);
  
  if(stream.is_open()){
  	getline(stream, line);
    std::istringstream myStream(line);
    while(myStream >> variable){
      if(count == 4 || count == 5){
      	IdleTimeInJiffies += stol(variable);
      }
    }
  }
  return IdleTimeInJiffies; 
}

// TODO: Read and return CPU utilization
long LinuxParser::CpuUtilization() {
  long PrevIdle = IdleJiffies();
  long PrevNonIdle = ActiveJiffies();
  long Idle = IdleJiffies();
  long NonIdle = ActiveJiffies();
  
  long PrevTotal = PrevIdle + PrevNonIdle;
  long Total = Idle + NonIdle;
  
  long totald = Total - PrevTotal;
  long idled = Idle - PrevIdle;
  
  long CPU_Percentage = (totald - idled)/totald;
  
  return CPU_Percentage;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key, value;
  int TotalNumberProcesses {};
  std::ifstream myProcesses(kProcDirectory + kMeminfoFilename);
  
  if(myProcesses.is_open()){
  	while(getline(myProcesses, line)){
    	std::istringstream myStream(line);
      while(myStream >> key >> value){
        if(key == "processes"){
        	TotalNumberProcesses = stoi(value);
        }
      }
    }
  }
  return TotalNumberProcesses; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  int TotalNumberRunning {};
  std::ifstream myProcesses(kProcDirectory + kMeminfoFilename);
  
  if(myProcesses.is_open()){
  	while(getline(myProcesses, line)){
    	std::istringstream myStream(line);
      while(myStream >> key >> value){
        if(key == "procs_running"){
        	TotalNumberRunning = stoi(value);
        }
      }
    }
  }
  return TotalNumberRunning;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line, path, type, value, CommandLine;
  std::ifstream myCommand(kProcDirectory + to_string(pid) + kCmdlineFilename);
  
  if(myCommand.is_open()){
  	getline(myCommand, line);
    std::istringstream myStream(line);
    myStream >> path >> type >> value;
  }
  CommandLine = path + type +value;
  return CommandLine; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line, key, value, RAM;
  int ram;
  std::ifstream myRam(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if(myRam.is_open()){
  	while(getline(myRam, line)){
    	std::istringstream myStream(line);
      	while(myStream >> key >> value){
        	if(key == "VmSize:"){
            	ram = stoi(value);
              	ram = ram / 1024;
              	RAM = to_string(ram);
            }
        }
    }
  }
  return RAM; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  string Uid;
  std::ifstream myUID(kProcDirectory + to_string(pid) + kStatusFilename);
  
  if(myUID.is_open()){
  	while(getline(myUID, line)){
      	std::istringstream myStream(line);
        while(myStream >> key >> value){
        	if(key == "Uid"){
            	Uid = value;
            }
        }
    }
  }
  return Uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line, CurrentUid, user, CurrentUser;
  char key;
  string UID = LinuxParser::Uid(pid);
  std::ifstream myUser(kPasswordPath);
  
  if(myUser.is_open()){
  	while(getline(myUser, line)){
      	std::replace(line.begin(),line.end(), ':' , ' ');
    	std::istringstream myStream(line);
      	while(myStream >> CurrentUser >> key >> CurrentUid){
        	if(CurrentUid == UID){
            	user = CurrentUser;
            }
        }
    }
  }
  
  return user; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, variable;
  long UpTime {}, UpTimeSeconds {};
  int count{0};
  std::ifstream ThatPID(kProcDirectory + to_string(pid) + kStatFilename);
  
  if(ThatPID.is_open()){
    getline(ThatPID, line);
    std::istringstream myStream(line);
    while(myStream >> variable){
  	   if(count == 21){
      	 UpTime = stol(variable);
       }
    count ++;
    }
  }
  UpTimeSeconds = UpTime / sysconf(_SC_CLK_TCK);
  
  return stol(Format::ElapsedTime(UpTimeSeconds)); 
}
