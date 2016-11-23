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

CentralNodeDriver::CentralNodeDriver(const char *portName, std::string configPath) :
  asynPortDriver(portName, 100, CENTRAL_NODE_DRIVER_NUM_PARAMS,
		 asynOctetMask | asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask | asynDrvUserMask, // interfaceMask
		 asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask, // interruptMask
		 ASYN_MULTIDEVICE,                       // asynFlags
		 1, 0, 0),            // autoConnect, priority, stackSize
  _configPath(configPath) {
#ifdef LOG_ENABLED
  centralNodeLogger = Loggers::getLogger("DRIVER");
#endif
  LOG_TRACE("DRIVER", "CentralNodeDriver constructor");

  createParam(CONFIG_LOAD_STRING, asynParamOctet, &_configLoadParam);
  createParam(DIGITAL_CHANNEL_STRING, asynParamInt32, &_digitalChannelParam);
}

CentralNodeDriver::~CentralNodeDriver() {
}

// asynStatus CentralNodeDriver::drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName,
// 					    size_t *psize) {
//   int addr;
//   getAddress(pasynUser, &addr);
//   LOG_TRACE("DRIVER", "drvUserCreate info=" << drvInfo << " timeout=" << pasynUser->timeout
// 	    << "; address=" << addr);
//   return asynSuccess;
// }

asynStatus CentralNodeDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual) {
  return asynSuccess;
}

asynStatus CentralNodeDriver::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual) {
  asynStatus status = asynSuccess;

  // Set the parameter in the parameter library
  status = setStringParam(pasynUser->reason, (char *) value);
  
  if (_configLoadParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Received request to load configuration: " << value);
    status = loadConfig(value);
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
  }
  *nActual = maxChars;

  return status;
}

asynStatus CentralNodeDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  int addr;
  getAddress(pasynUser, &addr);
  Engine::getInstance().getCurrentDb().digitalChannels()
  //  LOG_TRACE("DRIVER", "Reading input [address=" << addr << "]");
  *value = 1;
  return asynSuccess;
}

asynStatus CentralNodeDriver::loadConfig(const char *config) {
  std::string fullName = _configPath + config;
  try {
    if (Engine::getInstance().loadConfig(fullName) != 0) {
      std::cerr << "Failed to load configuration " << fullName << std::endl; 
      return asynError;
    }
  } catch (DbException ex) {
    std::cerr << "Exception loading config " << fullName << std::endl;
    std::cerr << ex.what() << std::endl;
    return asynError;
  }
  LOG_TRACE("DRIVER", "Loaded configuration: " << fullName);
  return asynSuccess;
}

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

