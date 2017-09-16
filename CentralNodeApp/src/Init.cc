#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <iocsh.h>
#include <epicsThread.h>
#include <initHooks.h>

#include "CentralNodeDriver.h"
#include "Log.h"

#include "central_node_database.h"
#include "central_node_engine.h"

#include <epicsExport.h> /* This should be the last header */

static CentralNodeDriver *pCNDriver = NULL;

static int checkPath(char *path, const char *var) {
  if (path == NULL) {
    fprintf(stderr, "ERROR: Missing environment variable \"%s\", can't proceed.\n", var);
    return -1;
  }

  struct stat s;
  int err = stat(path, &s);
  if (err == -1) {
    if (ENOENT == errno) {
      fprintf(stderr, "ERROR: Directory for %s=%s does not exist, can't proceed.\n",
	      var, path);
      return -1;
    }
    else {
      fprintf(stderr, "ERROR: Failed to get status for directory %s=%s, can't proceed.\n",
	      var, path);
      perror("stat");
      return -1;
    }
  }

  return 0;
}

static int configureCentralNode(const char *portName) {
  char *path = getenv(MPS_ENV_CONFIG_PATH);
  if (checkPath(path, MPS_ENV_CONFIG_PATH) != 0) {
    return -1;
  }

  char *fwFile = getenv(MPS_ENV_FW_CONFIG);
  if (checkPath(fwFile, MPS_ENV_FW_CONFIG) != 0) {
    return -1;
  }

  char *historyServer = getenv(MPS_ENV_HISTORY_HOST);
  if (historyServer == NULL) {
    fprintf(stderr, "ERROR: Missing environment variable \"%s\", can't proceed.\n", MPS_ENV_HISTORY_HOST);
    return -1;
  }
  char *historyPortStr = getenv(MPS_ENV_HISTORY_PORT);
  if (historyPortStr == NULL) {
    fprintf(stderr, "ERROR: Missing environment variable \"%s\", can't proceed.\n", MPS_ENV_HISTORY_PORT);
    return -1;
  }
  int historyPort = atoi(historyPortStr);

  Firmware::getInstance().loadConfig(fwFile);

  pCNDriver = new CentralNodeDriver(portName, path, historyServer, historyPort);

#if defined(LOG_ENABLED) && !defined(LOG_STDOUT)
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

/*=== mpsPrintQueue command =======================================================*/

static int mpsPrintQueue() {
  pCNDriver->printQueue();
  return 0;
}

static const iocshFuncDef mpsPrintQueueFuncDef = {"mpsPrintQueue", 0, 0};
static const iocshFuncDef mpspqFuncDef = {"mpspq", 0, 0};
static void mpsPrintQueueCallFunc(const iocshArgBuf *args) {
  mpsPrintQueue();
}

static void mpsPrintQueueRegistrar(void) {
  iocshRegister(&mpsPrintQueueFuncDef, mpsPrintQueueCallFunc);
  iocshRegister(&mpspqFuncDef, mpsPrintQueueCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsPrintQueueRegistrar);
}

/*=== mpsShowMitigation command =======================================================*/

static int mpsShowMitigation() {
  pCNDriver->showMitigation();
  return 0;
}

static const iocshFuncDef mpsShowMitigationFuncDef = {"mpsShowMitigation", 0, 0};
static void mpsShowMitigationCallFunc(const iocshArgBuf *args) {
  mpsShowMitigation();
}

static void mpsShowMitigationRegistrar(void) {
  iocshRegister(&mpsShowMitigationFuncDef, mpsShowMitigationCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowMitigationRegistrar);
}

/*=== mpsShowFaults command =======================================================*/

static int mpsShowFaults() {
  pCNDriver->showFaults();
  return 0;
}

static const iocshFuncDef mpsShowFaultsFuncDef = {"mpsShowFaults", 0, 0};
static void mpsShowFaultsCallFunc(const iocshArgBuf *args) {
  mpsShowFaults();
}

static void mpsShowFaultsRegistrar(void) {
  iocshRegister(&mpsShowFaultsFuncDef, mpsShowFaultsCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowFaultsRegistrar);
}

/*=== mpsShowFirmware command =======================================================*/

static int mpsShowFirmware() {
  pCNDriver->showFirmware();
  return 0;
}

static const iocshFuncDef mpsShowFirmwareFuncDef = {"mpsShowFirmware", 0, 0};
static const iocshFuncDef mpsfwFuncDef = {"mpsfw", 0, 0};
static void mpsShowFirmwareCallFunc(const iocshArgBuf *args) {
  mpsShowFirmware();
}

static void mpsShowFirmwareRegistrar(void) {
  iocshRegister(&mpsShowFirmwareFuncDef, mpsShowFirmwareCallFunc);
  iocshRegister(&mpsfwFuncDef, mpsShowFirmwareCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowFirmwareRegistrar);
}

/*=== mpsShowDatabaseInfo command =======================================================*/

static int mpsShowDatabaseInfo() {
  pCNDriver->showDatabaseInfo();
  return 0;
}

static const iocshFuncDef mpsShowDatabaseInfoFuncDef = {"mpsShowDatabaseInfo", 0, 0};
static const iocshFuncDef mpsdbFuncDef = {"mpsdb", 0, 0};
static void mpsShowDatabaseInfoCallFunc(const iocshArgBuf *args) {
  mpsShowDatabaseInfo();
}

static void mpsShowDatabaseInfoRegistrar(void) {
  iocshRegister(&mpsShowDatabaseInfoFuncDef, mpsShowDatabaseInfoCallFunc);
  iocshRegister(&mpsdbFuncDef, mpsShowDatabaseInfoCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowDatabaseInfoRegistrar);
}

/*=== mpsShowEngineInfo command =======================================================*/

static int mpsShowEngineInfo() {
  pCNDriver->showEngineInfo();
  return 0;
}

static const iocshFuncDef mpsShowEngineInfoFuncDef = {"mpsShowEngineInfo", 0, 0};
static const iocshFuncDef mpseiFuncDef = {"mpsei", 0, 0};
static void mpsShowEngineInfoCallFunc(const iocshArgBuf *args) {
  mpsShowEngineInfo();
}

static void mpsShowEngineInfoRegistrar(void) {
  iocshRegister(&mpsShowEngineInfoFuncDef, mpsShowEngineInfoCallFunc);
  iocshRegister(&mpseiFuncDef, mpsShowEngineInfoCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowEngineInfoRegistrar);
}

/*=== mpsShowUpdateBuffer command =======================================================*/

static const iocshArg mpssubArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssubArgs[1] = {&mpssubArg0};
static const iocshFuncDef mpsShowUpdateBufferFuncDef = {"mpsShowUpdateBuffer", 1, mpssubArgs};
static const iocshFuncDef mpssubFuncDef = {"mpssub", 1, mpssubArgs};
static void mpsShowUpdateBufferCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
  if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*appCard).second->name << ": ";
    std::cout << *(*appCard).second->applicationUpdateBuffer << std::endl;
  }
}

static void mpsShowUpdateBufferRegistrar(void) {
  iocshRegister(&mpsShowUpdateBufferFuncDef, mpsShowUpdateBufferCallFunc);
  iocshRegister(&mpssubFuncDef, mpsShowUpdateBufferCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowUpdateBufferRegistrar);
}
