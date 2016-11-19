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

  virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);

  virtual void report(FILE *fp, int details);

 private:
  MpsAsynParam *getParam(int key);

  ParamMap paramMap;
};

#endif
