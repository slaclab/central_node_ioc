#ifndef CENTRALNODEPARAMS_H
#define CENTRALNODEPARAMS_H

#include <asynParamType.h>

// Struct that holds register information. The id field
// is used by asyn to connect with the actual asyn parameter
typedef struct {
  asynParamType type; // Defined in asynPortDriver.h
  std::string name; // Name given in the INP/OUT asyn record field
  int id; // This is assigned by asynPortDriver::createParam()
} MpsAsynParam;


#endif
