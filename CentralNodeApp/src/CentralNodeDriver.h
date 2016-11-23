#ifndef CENTRALNODEDRIVER_H
#define CENTRALNODEDRIVER_H

#include <string>
#include <map>
#include <stdio.h>

#include <asynPortDriver.h>

// Environment variables
#define MPS_ENV_CONFIG_PATH "MPS_ENV_CONFIG_PATH"

// List of ASYN parameter names
#define CONFIG_LOAD_STRING       "CONFIG_LOAD"
#define DIGITAL_CHANNEL_STRING "DIGITAL_CHANNEL"
#define ANALOG_CHANNEL_STRING "ANALOG_CHANNEL"

const int CENTRAL_NODE_DRIVER_NUM_PARAMS = 3;

class CentralNodeDriver : public asynPortDriver {
public:
  CentralNodeDriver(const char *portname, std::string configPath);
  virtual ~CentralNodeDriver();

  /* virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName, */
  /* 				   size_t *psize); */
  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);

  virtual void report(FILE *fp, int details);

 private:
  std::string _configPath;

  // List of ASYN parameters
  int _configLoadParam;
  int _digitalChannelParam;
  int _analogChannelParam;

  asynStatus loadConfig(const char *config);
};

#endif
