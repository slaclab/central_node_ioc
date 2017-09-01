#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <iocsh.h>
#include <epicsThread.h>

#include "CentralNodeDriver.h"
#include "Log.h"

#include <epicsExport.h> /* This should be the last header */

static CentralNodeDriver *pCNDriver = NULL;

static int configureCentralNode(const char *portName) {
  char *path = getenv(MPS_ENV_CONFIG_PATH);

  if (path == NULL) {
    fprintf(stderr, "ERROR: Missing environment variable \"%s\", can't proceed.\n", MPS_ENV_CONFIG_PATH);
    return -1;
  }

  struct stat s;
  int err = stat(path, &s);
  if (err == -1) {
    if (ENOENT == errno) {
      fprintf(stderr, "ERROR: Directory for %s=%s does not exist, can't proceed.\n",
	      MPS_ENV_CONFIG_PATH, path);
      return -1;
    }
    else {
      fprintf(stderr, "ERROR: Failed to get status for config directory %s=%s, can't proceed.\n",
	      MPS_ENV_CONFIG_PATH, path);
      perror("stat");
      return -1;
    }
  }

  pCNDriver = new CentralNodeDriver(portName, path);

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

/*=== printQueue command =======================================================*/

static int printQueue() {
  pCNDriver->printQueue();
  return 0;
}

static const iocshFuncDef printQueueFuncDef = {"printQueue", 0, 0};
static void printQueueCallFunc(const iocshArgBuf *args) {
  printQueue();
}

static void printQueueRegistrar(void) {
  iocshRegister(&printQueueFuncDef, printQueueCallFunc);
}

extern "C" {
  epicsExportRegistrar(printQueueRegistrar);
}

/*=== showMitigation command =======================================================*/

static int showMitigation() {
  pCNDriver->showMitigation();
  return 0;
}

static const iocshFuncDef showMitigationFuncDef = {"showMitigation", 0, 0};
static void showMitigationCallFunc(const iocshArgBuf *args) {
  showMitigation();
}

static void showMitigationRegistrar(void) {
  iocshRegister(&showMitigationFuncDef, showMitigationCallFunc);
}

extern "C" {
  epicsExportRegistrar(showMitigationRegistrar);
}

/*=== showFaults command =======================================================*/

static int showFaults() {
  pCNDriver->showFaults();
  return 0;
}

static const iocshFuncDef showFaultsFuncDef = {"showFaults", 0, 0};
static void showFaultsCallFunc(const iocshArgBuf *args) {
  showFaults();
}

static void showFaultsRegistrar(void) {
  iocshRegister(&showFaultsFuncDef, showFaultsCallFunc);
}

extern "C" {
  epicsExportRegistrar(showFaultsRegistrar);
}

/*=== showFirmware command =======================================================*/

static int showFirmware() {
  pCNDriver->showFirmware();
  return 0;
}

static const iocshFuncDef showFirmwareFuncDef = {"showFirmware", 0, 0};
static void showFirmwareCallFunc(const iocshArgBuf *args) {
  showFirmware();
}

static void showFirmwareRegistrar(void) {
  iocshRegister(&showFirmwareFuncDef, showFirmwareCallFunc);
}

extern "C" {
  epicsExportRegistrar(showFirmwareRegistrar);
}
