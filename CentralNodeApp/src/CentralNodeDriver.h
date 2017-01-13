#ifndef CENTRALNODEDRIVER_H
#define CENTRALNODEDRIVER_H

#include <string>
#include <map>
#include <stdio.h>

#include <central_node_bypass.h>
#include <asynPortDriver.h>

// Environment variables
#define MPS_ENV_CONFIG_PATH "MPS_ENV_CONFIG_PATH"

// List of ASYN parameter names
#define CONFIG_LOAD_STRING                     "CONFIG_LOAD"
#define DEVICE_INPUT_STRING                    "DEVICE_INPUT"
#define ANALOG_DEVICE_STRING                   "ANALOG_DEVICE"
#define MITIGATION_DEVICE_STRING               "MITIGATION_DEVICE"
#define FAULT_STRING                           "FAULT"
#define FAULT_IGNORED_STRING                   "FAULT_IGNORED"
#define DEVICE_INPUT_LATCHED_STRING            "DEVICE_INPUT_LATCHED"
#define DEVICE_INPUT_UNLATCH_STRING            "DEVICE_INPUT_UNLATCH"
#define DEVICE_INPUT_BYPV_STRING               "DEVICE_INPUT_BYPV"
#define DEVICE_INPUT_BYPS_STRING               "DEVICE_INPUT_BYPS"
#define DEVICE_INPUT_BYPEXPDATE_STRING         "DEVICE_INPUT_BYPEXPDATE"
#define DEVICE_INPUT_BYPEXPDATE_STRING_STRING  "DEVICE_INPUT_BYPEXPDATE_STRING"
#define ANALOG_DEVICE_LATCHED_STRING           "ANALOG_DEVICE_LATCHED"
#define ANALOG_DEVICE_UNLATCH_STRING           "ANALOG_DEVICE_UNLATCH"
#define ANALOG_DEVICE_BYPV_STRING              "ANALOG_DEVICE_BYPV"
#define ANALOG_DEVICE_BYPS_STRING              "ANALOG_DEVICE_BYPS"
#define ANALOG_DEVICE_BYPEXPDATE_STRING        "ANALOG_DEVICE_BYPEXPDATE"
#define ANALOG_DEVICE_BYPEXPDATE_STRING_STRING "ANALOG_DEVICE_BYPEXPDATE_STRING"

#define TEST_DEVICE_INPUT_STRING               "TEST_DEVICE_INPUT"
#define TEST_ANALOG_DEVICE_STRING              "TEST_ANALOG_DEVICE"
#define TEST_CHECK_FAULTS_STRING               "TEST_CHECK_FAULTS"
#define TEST_CHECK_BYPASS_STRING               "TEST_CHECK_BYPASS"

const int CENTRAL_NODE_DRIVER_NUM_PARAMS = 22;

class CentralNodeDriver : public asynPortDriver {
public:
  CentralNodeDriver(const char *portname, std::string configPath);
  virtual ~CentralNodeDriver();

  /* virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName, */
  /* 				   size_t *psize); */
  //  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask);
  virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);

  virtual void report(FILE *fp, int details);

  void printQueue();
  
 private:
  std::string _configPath;

  // List of ASYN parameters
  int _configLoadParam;
  int _deviceInputParam;
  int _analogDeviceParam;
  int _mitigationDeviceParam;
  int _faultParam;
  int _faultIgnoredParam;
  int _deviceInputLatchedParam;
  int _deviceInputUnlatchParam;
  int _deviceInputBypassValueParam;
  int _deviceInputBypassStatusParam;
  int _deviceInputBypassExpirationDateParam;
  int _deviceInputBypassExpirationDateStringParam;
  int _analogDeviceLatchedParam;
  int _analogDeviceUnlatchParam;
  int _analogDeviceBypassValueParam;
  int _analogDeviceBypassStatusParam;
  int _analogDeviceBypassExpirationDateParam;
  int _analogDeviceBypassExpirationDateStringParam;

  int _testDeviceInputParam;
  int _testAnalogDeviceParam;
  int _testCheckFaultsParam;
  int _testCheckBypassParam;

  asynStatus loadConfig(const char *config);
  asynStatus loadTestDeviceInputs(const char *testFileName);
  asynStatus loadTestAnalogDevices(const char *testFileName);

  asynStatus setBypass(BypassType bypassType, int deviceId, epicsInt32 expirationTime);
};

#endif
