#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

#include <iocsh.h>
#include <epicsThread.h>
#include <initHooks.h>
#include <yamlLoader.h>

#include "CentralNodeDriver.h"
#include "Log.h"

#include "central_node_database_tables.h"
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

  Path root = cpswGetRoot();
  if (!root) {
    std::cout << "There is no root from yaml loader, creating root now (file=" 
	      << fwFile << ")" << std::endl;
    Firmware::getInstance().createRoot(fwFile);
  }
  else {
    Firmware::getInstance().setRoot(root);
  }
  Firmware::getInstance().createRegisters();

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
static const iocshFuncDef mpssmFuncDef = {"mpssm", 0, 0};
static void mpsShowMitigationCallFunc(const iocshArgBuf *args) {
  mpsShowMitigation();
}

static void mpsShowMitigationRegistrar(void) {
  iocshRegister(&mpsShowMitigationFuncDef, mpsShowMitigationCallFunc);
  iocshRegister(&mpssmFuncDef, mpsShowMitigationCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowMitigationRegistrar);
}

/*=== mpsShowFaults command =======================================================*/

static int mpsShowFaults() {
  pCNDriver->showFaults();
  return 0;
}

static const iocshArg mpssfArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssfArgs[1] = {&mpssfArg0};
static const iocshFuncDef mpsShowFaultsFuncDef = {"mpsShowFaults", 1, mpssfArgs};
static const iocshFuncDef mpssfFuncDef = {"mpssf", 1, mpssfArgs};
static void mpsShowFaultsCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  if (id != 0) {
    Engine::getInstance().getCurrentDb()->lock();
    DbFaultMap::iterator fault = Engine::getInstance().getCurrentDb()->faults->find(id);
    
    if (fault != Engine::getInstance().getCurrentDb()->faults->end()) {
      std::cout << (*fault).second << std::endl;
    }
    Engine::getInstance().getCurrentDb()->unlock();
  }
  else {
    mpsShowFaults();
  }
}

static void mpsShowFaultsRegistrar(void) {
  iocshRegister(&mpsShowFaultsFuncDef, mpsShowFaultsCallFunc);
  iocshRegister(&mpssfFuncDef, mpsShowFaultsCallFunc);
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

  if (id == 999) {
    std::cout << "Full update buffer:" << std::endl;
    ApplicationUpdateBufferFullBitSet *buf = reinterpret_cast<ApplicationUpdateBufferFullBitSet *>(Engine::getInstance().getCurrentDb()->getFastUpdateBuffer());
    std::cout << *buf << std::endl;
  }
  else {
    DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
    if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
      std::cout << (*appCard).second->name << " [id:"
		<< (*appCard).second->globalId << "]:" << std::endl;

      std::cout << "WasLow: " << std::endl;
      std::cout << *(*appCard).second->wasLowBuffer << std::endl;
      std::cout << "WasHigh: " << std::endl;
      std::cout << *(*appCard).second->wasHighBuffer << std::endl;
    }
  }
}

static void mpsShowUpdateBufferRegistrar(void) {
  iocshRegister(&mpsShowUpdateBufferFuncDef, mpsShowUpdateBufferCallFunc);
  iocshRegister(&mpssubFuncDef, mpsShowUpdateBufferCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowUpdateBufferRegistrar);
}

/*=== mpsShowConfigBuffer command =======================================================*/

static const iocshArg mpsscbArg0 = {"id", iocshArgInt};
static const iocshArg * const mpsscbArgs[1] = {&mpsscbArg0};
static const iocshFuncDef mpsShowConfigBufferFuncDef = {"mpsShowConfigBuffer", 1, mpsscbArgs};
static const iocshFuncDef mpsscbFuncDef = {"mpsscb", 1, mpsscbArgs};
static void mpsShowConfigBufferCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
  if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*appCard).second->name << ": ";
    std::cout << *(*appCard).second->applicationConfigBuffer << std::endl;
    //    (*appCard).second->printAnalogConfiguration();
  }
}

static void mpsShowConfigBufferRegistrar(void) {
  iocshRegister(&mpsShowConfigBufferFuncDef, mpsShowConfigBufferCallFunc);
  iocshRegister(&mpsscbFuncDef, mpsShowConfigBufferCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowConfigBufferRegistrar);
}

/*=== mpsShowDigitalDevice command =======================================================*/

static const iocshArg mpssddArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssddArgs[1] = {&mpssddArg0};
static const iocshFuncDef mpsShowDigitalDeviceFuncDef = {"mpsShowDigitalDevice", 1, mpssddArgs};
static const iocshFuncDef mpssddFuncDef = {"mpssdd", 1, mpssddArgs};
static void mpsShowDigitalDeviceCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  Engine::getInstance().getCurrentDb()->lock();
  DbDigitalDeviceMap::iterator device = Engine::getInstance().getCurrentDb()->digitalDevices->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->digitalDevices->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

static void mpsShowDigitalDeviceRegistrar(void) {
  iocshRegister(&mpsShowDigitalDeviceFuncDef, mpsShowDigitalDeviceCallFunc);
  iocshRegister(&mpssddFuncDef, mpsShowDigitalDeviceCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowDigitalDeviceRegistrar);
}

/*=== mpsShowDeviceInput command =======================================================*/

static const iocshArg mpssdiArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssdiArgs[1] = {&mpssdiArg0};
static const iocshFuncDef mpsShowDeviceInputFuncDef = {"mpsShowDeviceInput", 1, mpssdiArgs};
static const iocshFuncDef mpssdiFuncDef = {"mpssdi", 1, mpssdiArgs};
static void mpsShowDeviceInputCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  Engine::getInstance().getCurrentDb()->lock();
  DbDeviceInputMap::iterator device = Engine::getInstance().getCurrentDb()->deviceInputs->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

static void mpsShowDeviceInputRegistrar(void) {
  iocshRegister(&mpsShowDeviceInputFuncDef, mpsShowDeviceInputCallFunc);
  iocshRegister(&mpssdiFuncDef, mpsShowDeviceInputCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowDeviceInputRegistrar);
}

/*=== mpsShowAnalogDevice command =======================================================*/

static const iocshArg mpssadArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssadArgs[1] = {&mpssadArg0};
static const iocshFuncDef mpsShowAnalogDeviceFuncDef = {"mpsShowAnalogDevice", 1, mpssadArgs};
static const iocshFuncDef mpssadFuncDef = {"mpssad", 1, mpssadArgs};
static void mpsShowAnalogDeviceCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  Engine::getInstance().getCurrentDb()->lock();
  DbAnalogDeviceMap::iterator device = Engine::getInstance().getCurrentDb()->analogDevices->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->analogDevices->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

static void mpsShowAnalogDeviceRegistrar(void) {
  iocshRegister(&mpsShowAnalogDeviceFuncDef, mpsShowAnalogDeviceCallFunc);
  iocshRegister(&mpssadFuncDef, mpsShowAnalogDeviceCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowAnalogDeviceRegistrar);
}

/*=== mpsShowMitigationDevice command =======================================================*/

static const iocshArg mpssmdArg0 = {"id", iocshArgInt};
static const iocshArg * const mpssmdArgs[1] = {&mpssmdArg0};
static const iocshFuncDef mpsShowMitigationDeviceFuncDef = {"mpsShowMitigationDevice", 1, mpssmdArgs};
static const iocshFuncDef mpssmdFuncDef = {"mpssmd", 1, mpssmdArgs};
static void mpsShowMitigationDeviceCallFunc(const iocshArgBuf *args) {
  int id = args[0].ival;  

  Engine::getInstance().getCurrentDb()->lock();
  DbMitigationDeviceMap::iterator device = Engine::getInstance().getCurrentDb()->mitigationDevices->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->mitigationDevices->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

static void mpsShowMitigationDeviceRegistrar(void) {
  iocshRegister(&mpsShowMitigationDeviceFuncDef, mpsShowMitigationDeviceCallFunc);
  iocshRegister(&mpssmdFuncDef, mpsShowMitigationDeviceCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsShowMitigationDeviceRegistrar);
}
