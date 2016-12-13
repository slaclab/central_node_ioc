#ifndef CENTRALNODEDRIVER_H
#define CENTRALNODEDRIVER_H

#include <string>
#include <map>
#include <stdio.h>

#include <asynPortDriver.h>

// Environment variables
#define MPS_ENV_CONFIG_PATH "MPS_ENV_CONFIG_PATH"

// List of ASYN parameter names
#define CONFIG_LOAD_STRING        "CONFIG_LOAD"
#define DEVICE_INPUT_STRING       "DEVICE_INPUT"
#define ANALOG_DEVICE_STRING      "ANALOG_DEVICE"
#define MITIGATION_DEVICE_STRING  "MITIGATION_DEVICE"
#define FAULT_STRING              "FAULT"

#define TEST_DEVICE_INPUT_STRING  "TEST_DEVICE_INPUT"
#define TEST_ANALOG_DEVICE_STRING "TEST_ANALOG_DEVICE"
#define TEST_CHECK_FAULTS_STRING  "TEST_CHECK_FAULTS"

const int CENTRAL_NODE_DRIVER_NUM_PARAMS = 8;

class CentralNodeDriver : public asynPortDriver {
public:
  CentralNodeDriver(const char *portname, std::string configPath);
  virtual ~CentralNodeDriver();

  /* virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName, */
  /* 				   size_t *psize); */
  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask);

  virtual void report(FILE *fp, int details);

 private:
  std::string _configPath;

  // List of ASYN parameters
  int _configLoadParam;
  int _deviceInputParam;
  int _analogDeviceParam;
  int _mitigationDeviceParam;
  int _faultParam;

  int _testDeviceInputParam;
  int _testAnalogDeviceParam;
  int _testCheckFaultsParam;

  asynStatus loadConfig(const char *config);
  asynStatus loadTestDeviceInputs(const char *testFileName);
  asynStatus loadTestAnalogDevices(const char *testFileName);
};

#endif
