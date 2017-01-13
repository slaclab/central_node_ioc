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
  createParam(FAULT_STRING, asynParamUInt32Digital, &_faultParam);
  createParam(FAULT_IGNORED_STRING, asynParamUInt32Digital, &_faultIgnoredParam);
  createParam(DEVICE_INPUT_LATCHED_STRING, asynParamUInt32Digital, &_deviceInputLatchedParam);
  createParam(DEVICE_INPUT_UNLATCH_STRING, asynParamUInt32Digital, &_deviceInputUnlatchParam);
  createParam(DEVICE_INPUT_BYPV_STRING, asynParamUInt32Digital, &_deviceInputBypassValueParam);
  createParam(DEVICE_INPUT_BYPS_STRING, asynParamUInt32Digital, &_deviceInputBypassStatusParam);
  createParam(DEVICE_INPUT_BYPEXPDATE_STRING, asynParamInt32, &_deviceInputBypassExpirationDateParam);
  createParam(DEVICE_INPUT_BYPEXPDATE_STRING_STRING, asynParamOctet, &_deviceInputBypassExpirationDateStringParam);
  createParam(ANALOG_DEVICE_LATCHED_STRING, asynParamInt32, &_analogDeviceLatchedParam);
  createParam(ANALOG_DEVICE_UNLATCH_STRING, asynParamUInt32Digital, &_analogDeviceUnlatchParam);
  createParam(ANALOG_DEVICE_BYPV_STRING, asynParamInt32, &_analogDeviceBypassValueParam);
  createParam(ANALOG_DEVICE_BYPS_STRING, asynParamUInt32Digital, &_analogDeviceBypassStatusParam);
  createParam(ANALOG_DEVICE_BYPEXPDATE_STRING, asynParamInt32, &_analogDeviceBypassExpirationDateParam);
  createParam(ANALOG_DEVICE_BYPEXPDATE_STRING_STRING, asynParamOctet, &_analogDeviceBypassExpirationDateStringParam);

  createParam(TEST_DEVICE_INPUT_STRING, asynParamOctet, &_testDeviceInputParam);
  createParam(TEST_ANALOG_DEVICE_STRING, asynParamOctet, &_testAnalogDeviceParam);
  createParam(TEST_CHECK_FAULTS_STRING, asynParamInt32, &_testCheckFaultsParam);
  createParam(TEST_CHECK_BYPASS_STRING, asynParamInt32, &_testCheckBypassParam);

  // Initialize bypass date strings
  for (int i = 0; i < 100; ++i) {
    setStringParam(i, _deviceInputBypassExpirationDateStringParam, "Bypass date not set");
    setStringParam(i, _analogDeviceBypassExpirationDateStringParam, "Bypass date not set");
  }
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
/*
asynStatus CentralNodeDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual) {
  std::cout << "READOCTET"<< std::endl;
  LOG_TRACE("DRIVER", "ReadOctet reason=" << pasynUser->reason);
  if (_deviceInputBypassExpirationDateStringParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Read bypass string size (" << maxChars << ")");
  }
  *nActual = maxChars;
  return asynSuccess;
}
*/
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
  int addr;
  getAddress(pasynUser, &addr);

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized, reason="
	      << pasynUser->reason << ", addr=" << addr << ", value=" << value);
    return setIntegerParam(addr, pasynUser->reason, value);
  }

  LOG_TRACE("DRIVER", "INFO: reason=" << pasynUser->reason << ", addr=" << addr << ", value=" << value);
  if (_testCheckFaultsParam == pasynUser->reason) {
    Engine::getInstance().checkFaults();
  }
  else if (_deviceInputBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_DIGITAL, addr, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _deviceInputBypassExpirationDateParam, 0);
    }
  }
  else if (_analogDeviceBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_ANALOG, addr, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _analogDeviceBypassExpirationDateParam, 0);
    }
  }
  else if (_testCheckBypassParam == pasynUser->reason) {
    Engine::getInstance().getBypassManager()->checkBypassQueue();
  }
  else if (_analogDeviceBypassValueParam == pasynUser->reason) {
    Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass->value = value;
    LOG_TRACE("DRIVER", "BypassValue: "
	      << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->channel->name
	      << " value: " << value);
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  // TODO: check if 'addr' is valid
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized");
    //    return asynError;
    return status;
  }

  if (_mitigationDeviceParam == pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass) {
      *value = Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass->number;
    }
    else {
      LOG_TRACE("DRIVER", "ERROR: Invalid allowed class for mitigation device "
		<< Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->name);
    }
    //      LOG_TRACE("DRIVER", "Mitagation: " << Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->name);// << ": " << Engine::getInstance().getCurrentDb()->mitigationDevices->at(addr)->allowedBeamClass->number);
  }
  else if (_analogDeviceLatchedParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->latchedValue;
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request (reason " << pasynUser->reason << ")");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask) {
  // TODO: check if 'addr' is valid
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized");
    return status;
    //    return asynError;
  }

  if (_deviceInputParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->value;
    //      LOG_TRACE("DRIVER", "Input: " << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name << ": " << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->value);
  }
  else if (_analogDeviceParam == pasynUser->reason) {
    // LOG_TRACE("DRIVER", "ANALOG value=" << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->value
    // 	      << " mask=" << mask);
    *value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->value & mask;
  }
  else if (_faultParam == pasynUser->reason) {
    *value = 0;
    if (Engine::getInstance().getCurrentDb()->faults->at(addr)->faulted) {
      *value = 1;
    }
  }
  else if (_faultIgnoredParam == pasynUser->reason) {
    *value = 0;
    if (Engine::getInstance().getCurrentDb()->faults->at(addr)->ignored) {
      *value = 1;
    }
  }
  else if (_deviceInputLatchedParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->latchedValue;
  }
  else if (_deviceInputBypassStatusParam ==  pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->status == BYPASS_VALID) {
      *value = 1;
    }
    else {
      *value = 0;
    }
  }
  else if (_analogDeviceBypassStatusParam ==  pasynUser->reason) {
    if (Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass->status == BYPASS_VALID) {
      *value = 1;
    }
    else {
      *value = 0;
    }
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask) {
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized (reason=" << pasynUser->reason
	      << ", addr=" << addr << ")");
    status = setUIntDigitalParam(addr, pasynUser->reason, value, mask);
    return status;
  }

  if (_deviceInputUnlatchParam == pasynUser->reason) {
    int faultValue = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->faultValue;
    faultValue == 0? faultValue = 1 : faultValue = 0; // Flip faultValue and assign to latchedValue
    Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->latchedValue = faultValue;
    status = setUIntDigitalParam(addr, pasynUser->reason, value, mask);
    LOG_TRACE("DRIVER", "Unlatch: "
	      << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name
	      << " value: " << faultValue);
  }
  else if (_analogDeviceUnlatchParam == pasynUser->reason) {
    Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->latchedValue = 0; // Clear all threshold faults
    status = setUIntDigitalParam(addr, pasynUser->reason, value, mask);
    LOG_TRACE("DRIVER", "Unlatch: "
	      << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->channel->name
	      << " value: " << 0);
  }
  else if (_deviceInputBypassValueParam == pasynUser->reason) {
    Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->value = value;
    LOG_TRACE("DRIVER", "BypassValue: "
	      << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name
	      << " value: " << value);
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

/**
 * @param expirationTime bypass expiration time in seconds since now. If expirationTime is zero or negatie
 *                       the bypass is cancelled.
 */
asynStatus CentralNodeDriver::setBypass(BypassType bypassType, int deviceId, epicsInt32 expirationTime) {
  asynStatus status = asynSuccess;
  time_t now;
  time(&now);
  /*
  if (expirationTime > 0 && expirationTime < now) {
    std::cerr << "WARNING: Invalid expiration time " << expirationTime
	      << ". Must be greater than (now) " << now << "." << std::endl;
    return asynError;
  }
  */

  // Add expirationTime to current time, unless the bypass is being cancelled
  if (expirationTime > 0) {
    expirationTime += now;
  }
  else {
    expirationTime = 0;
  }

  uint32_t bypassValue = Engine::getInstance().getCurrentDb()->deviceInputs->at(deviceId)->bypass->value;
  Engine::getInstance().getBypassManager()->setBypass(Engine::getInstance().getCurrentDb(), bypassType,
						      deviceId, bypassValue, expirationTime);

  if (expirationTime == 0) {
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(deviceId, _deviceInputBypassExpirationDateStringParam, "Not Bypassed");
    }
    else {
      status = setStringParam(deviceId, _analogDeviceBypassExpirationDateStringParam, "Not Bypassed");
    }
  }
  else {
    time_t expTime = expirationTime;
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(deviceId, _deviceInputBypassExpirationDateStringParam, ctime(&expTime));
    }
    else {
      status = setStringParam(deviceId, _analogDeviceBypassExpirationDateStringParam, ctime(&expTime));
    }
  }

  return status;
}

void CentralNodeDriver::printQueue() {
  Engine::getInstance().getBypassManager()->printBypassQueue();
}

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

