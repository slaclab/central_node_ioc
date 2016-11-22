#ifndef CENTRALNODEDRIVER_H
#define CENTRALNODEDRIVER_H

#include <string>
#include <map>
#include <stdio.h>
#include <asynPortDriver.h>

#include "CentralNodeParam.h"

typedef std::map<int, MpsAsynParam *> ParamMap;

class CentralNodeDriver : public asynPortDriver {
public:
  CentralNodeDriver(const char *portname);
  virtual ~CentralNodeDriver();

  virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName,
				   size_t *psize);
  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);

  virtual void report(FILE *fp, int details);

 private:
  MpsAsynParam *getParam(int key);

  ParamMap paramMap;
};

#endif
