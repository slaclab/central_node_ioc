#include "CentralNodeDriver.h"

#include <iostream>
#include <sstream>

#include <epicsTypes.h>
#include <epicsExport.h>
#include <drvSup.h>
#include <dbCommon.h>
#include <alarm.h>

#include "LogWrapper.h"

#include <central_node_engine.h>

#ifdef LOG_ENABLED
using namespace easyloggingpp;
static Logger *centralNodeLogger;
#endif

// Number of asyn parameters
const int CENTRALNODE_NUM_PARAMS = 2;

// Information extracted from the input file
MpsAsynParam CentralNodeParams[CENTRALNODE_NUM_PARAMS] = {
  {asynParamOctet, "ALGLOAD", -1},
  {asynParamInt32, "DIGITAL_INPUT", -1},
};

CentralNodeDriver::CentralNodeDriver(const char *portName) :
  asynPortDriver(portName, 100, CENTRALNODE_NUM_PARAMS,
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

asynStatus CentralNodeDriver::drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName,
					    size_t *psize) {
  int addr;
  getAddress(pasynUser, &addr);
  LOG_TRACE("DRIVER", "drvUserCreate info=" << drvInfo << " timeout=" << pasynUser->timeout
	    << "; address=" << addr);
  return asynSuccess;
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

asynStatus CentralNodeDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  int addr;
  getAddress(pasynUser, &addr);
  Engine::getInstance();
  LOG_TRACE("DRIVER", "Reading input [address=" << addr << "]");
  *value = 1;
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

