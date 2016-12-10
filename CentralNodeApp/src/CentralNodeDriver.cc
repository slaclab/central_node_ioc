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
		 asynOctetMask | asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
		 asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask, // interruptMask
		 ASYN_MULTIDEVICE,                       // asynFlags
		 1, 0, 0),            // autoConnect, priority, stackSize
  _configPath(configPath) {
#ifdef LOG_ENABLED
  centralNodeLogger = Loggers::getLogger("DRIVER");
#endif
  LOG_TRACE("DRIVER", "CentralNodeDriver constructor");

  createParam(CONFIG_LOAD_STRING, asynParamOctet, &_configLoadParam);
  createParam(DEVICE_INPUT_STRING, asynParamUInt32Digital, &_deviceInputParam);
  createParam(ANALOG_DEVICE_STRING, asynParamUInt32Digital, &_analogDeviceParam);
  createParam(MITIGATION_DEVICE_STRING, asynParamInt32, &_mitigationDeviceParam);

  createParam(TEST_DEVICE_INPUT_STRING, asynParamOctet, &_testDeviceInputParam);
  createParam(TEST_ANALOG_DEVICE_STRING, asynParamOctet, &_testAnalogDeviceParam);
  createParam(TEST_CHECK_FAULTS_STRING, asynParamInt32, &_testCheckFaultsParam);
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
  else if (_testDeviceInputParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test device inputs from file: " << value);
    status = loadTestDeviceInputs(value);
  }
  else if (_testAnalogDeviceParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test analog devices from file: " << value);
    status = loadTestAnalogDevices(value);
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
  }
  *nActual = maxChars;

  return status;
}

asynStatus CentralNodeDriver::writeInt32(asynUser *pasynUser, epicsInt32 value) {
  asynStatus status = asynSuccess;
  if (_testCheckFaultsParam == pasynUser->reason) {
    Engine::getInstance().checkFaults();
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);
  if (_mitigationDeviceParam == pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()) {
      // TODO: check if 'addr' is valid
      if (Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass) {
	*value = Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass->number;
      }
      else {
	LOG_TRACE("DRIVER", "ERROR: Invalid allowed class for mitigation device "
		  << Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->name);
      }
	//      LOG_TRACE("DRIVER", "Mitagation: " << Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->name);// << ": " << Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass->number);
    }
    else {
      // Database has not been loaded
      LOG_TRACE("DRIVER", "ERROR: Invalid database");
      status = asynError;
    }
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request (reason " << pasynUser->reason << ")");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask) {
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);
  if (_deviceInputParam == pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()) {
      // TODO: check if 'addr' is valid
      *value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->value;
      //      LOG_TRACE("DRIVER", "Input: " << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name << ": " << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->value);
    }
    else {
      // Database has not been loaded
      LOG_TRACE("DRIVER", "ERROR: Invalid database");
      status = asynError;
    }
  }
  else if (_analogDeviceParam == pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()) {
      // TODO: check if 'addr' is valid
      *value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->value & mask;
    }
    else {
      // Database has not been loaded
      LOG_TRACE("DRIVER", "ERROR: Invalid database");
      status = asynError;
    }
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::loadConfig(const char *config) {
  std::string fullName = _configPath + config;
  try {
    if (Engine::getInstance().loadConfig(fullName) != 0) {
      std::cerr << "ERROR: Failed to load configuration " << fullName << std::endl; 
      return asynError;
    }
  } catch (DbException ex) {
    std::cerr << "ERROR: Exception loading config " << fullName << std::endl;
    std::cerr << ex.what() << std::endl;
    return asynError;
  }
  LOG_TRACE("DRIVER", "Loaded configuration: " << fullName);
  return asynSuccess;
}

asynStatus CentralNodeDriver::loadTestDeviceInputs(const char *testFilename) {
  asynStatus status = asynSuccess;
  std::string fullName = _configPath + "../inputs/" + testFilename;

  std::ifstream testInputFile;
  testInputFile.open(fullName.c_str(), std::ifstream::in);
  if (!testInputFile.is_open()) {
    std::cerr << "ERROR: Failed to load test input file " << fullName << std::endl; 
    return asynError;
  }

  while (!testInputFile.eof()) {
    int deviceId;
    int deviceValue;
    testInputFile >> deviceId;
    testInputFile >> deviceValue;

    try {
      Engine::getInstance().getCurrentDb()->deviceInputs->at(deviceId)->update(deviceValue);
    } catch (std::exception ex) {
      std::cerr << "ERROR: Invalid device input ID: " << deviceId << std::endl;
      status = asynError;
    }
  } 

  return status;
}

asynStatus CentralNodeDriver::loadTestAnalogDevices(const char *testFilename) {
  asynStatus status = asynSuccess;
  std::string fullName = _configPath + "../inputs/" + testFilename;

  std::ifstream testInputFile;
  testInputFile.open(fullName.c_str(), std::ifstream::in);
  if (!testInputFile.is_open()) {
    std::cerr << "ERROR: Failed to load test input file " << fullName << std::endl; 
    return asynError;
  }

  while (!testInputFile.eof()) {
    int deviceId;
    int deviceValue;
    testInputFile >> deviceId;
    testInputFile >> deviceValue;

    try {
      Engine::getInstance().getCurrentDb()->analogDevices->at(deviceId)->update(deviceValue);
    } catch (std::exception ex) {
      std::cerr << "ERROR: Invalid analog device ID: " << deviceId << std::endl;
      status = asynError;
    }
  } 

  return status;
}

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

