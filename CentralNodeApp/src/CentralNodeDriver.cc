#include "CentralNodeDriver.h"

#include <iostream>
#include <sstream>

#include <epicsTypes.h>
#include <epicsExport.h>
#include <drvSup.h>
#include <dbCommon.h>
#include <alarm.h>

#include "Log.h"
#include "LogWrapper.h"

#ifdef LOG_ENABLED
using namespace easyloggingpp;
static Logger *centralNodeLogger;
#endif

// Number of asyn parameters
const int CENTRALNODE_NUM_PARAMS = 1;

// Information extracted from the input file
MpsAsynParam CentralNodeParams[CENTRALNODE_NUM_PARAMS] = {
  {asynParamOctet, "ALGLOAD", -1},
};

CentralNodeDriver::CentralNodeDriver(const char *portName) :
  asynPortDriver(portName, 1, CENTRALNODE_NUM_PARAMS,
		 asynOctetMask | asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask | asynDrvUserMask, // interfaceMask
		 asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask, // interruptMask
		 ASYN_MULTIDEVICE,                       // asynFlags
		 1, 0, 0) {           // autoConnect, priority, stackSize
#ifdef LOG_ENABLED
  centralNodeLogger = Loggers::getLogger("DRIVER");
#endif
  LOG_TRACE("DRIVER", "CentralNodeDriver constructor");
  for (int i = 0; i < CENTRALNODE_NUM_PARAMS; i++) {
    // If the param belons to the Fast ADC block, change the blockId
    // to make it possible to reflesh all waveforms independently
    LOG_TRACE("DRIVER", "Creating parameter [" << i << "] " << CentralNodeParams[i].name);
    createParam(CentralNodeParams[i].name.c_str(), CentralNodeParams[i].type, &CentralNodeParams[i].id);
    paramMap.insert(std::pair<int, MpsAsynParam *>(CentralNodeParams[i].id, &CentralNodeParams[i]));
  }
}

CentralNodeDriver::~CentralNodeDriver() {
}

asynStatus CentralNodeDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual) {
  return asynSuccess;
}

asynStatus CentralNodeDriver::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual) {
  // First find the parameter in the _paramMap using the reason (key) field
  MpsAsynParam *param = getParam(pasynUser->reason);

  LOG_TRACE("DRIVER", "Writing to " << param->name << " the string " << value);
  *nActual = maxChars;
  return asynSuccess;
}

MpsAsynParam *CentralNodeDriver::getParam(int key) {
  ParamMap::iterator it = paramMap.find(key);
  if (it == paramMap.end()) {
    return NULL;
  }

  return it->second;
}  

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

