#include <iostream>

#include <iocsh.h>
#include <epicsThread.h>

#include "CentralNodeDriver.h"
#include "Log.h"

#include <epicsExport.h> /* This should be the last header */

static CentralNodeDriver *pCNDriver;

static int configureCentralNode(const char *portName) {
  pCNDriver = new CentralNodeDriver(portName);

#ifdef LOG_ENABLED
  Configurations c;
  //  c.parseFromText("*GLOBAL:\n Format = %level %msg");
  Loggers::setDefaultConfigurations(c, true);
#endif

  return 0;
}

static const iocshArg configArg0 = { "port name", iocshArgString};
static const iocshArg * const configArgs[] = {&configArg0};
static const iocshFuncDef configFuncDef = {"configureCentralNode", 1, configArgs};
static void configCallFunc(const iocshArgBuf *args) {
  configureCentralNode(args[0].sval);
}

static void initCentralNodeRegistrar(void) {
  iocshRegister(&configFuncDef, configCallFunc);
}

extern "C" {
epicsExportRegistrar(initCentralNodeRegistrar);
}
