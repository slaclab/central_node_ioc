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
  bool testIoc = false;
  char hostname[100];
  if (gethostname(hostname, 50) == 0) {
    std::string hoststr = hostname;
    if (!hoststr.compare(0, 9, "lcls-dev3")) {
      testIoc = true;
    }
  }

  char *path = getenv(MPS_ENV_CONFIG_PATH);
  if (checkPath(path, MPS_ENV_CONFIG_PATH) != 0) {
    return -1;
  }

  char *fwFile = getenv(MPS_ENV_FW_CONFIG);
  if (!testIoc) {
    if (checkPath(fwFile, MPS_ENV_FW_CONFIG) != 0) {
      return -1;
    }
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

  if (!testIoc) {
    Path root = cpswGetRoot();
    if (!root) {
      std::cout << "There is no root from yaml loader, creating root now (file=" 
		<< fwFile << ")" << std::endl;
      Firmware::getInstance().createRoot(fwFile);
    }
    else {
      Firmware::getInstance().setRoot(root);
    }
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

/*=== mpsShowMitigation command =======================================================*/

static int mpsShowMitigation() {
  pCNDriver->showMitigation();
  return 0;
}

/*=== mpsShowFaults command =======================================================*/

static int mpsShowFaults() {
  pCNDriver->showFaults();
  return 0;
}

static void mpsShowFault(int id) {
  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbFaultMap::iterator fault = Engine::getInstance().getCurrentDb()->faults->find(id);
  
  if (fault != Engine::getInstance().getCurrentDb()->faults->end()) {
    std::cout << (*fault).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowFirmware command =======================================================*/

static int mpsShowFirmware() {
  pCNDriver->showFirmware();
  return 0;
}

/*=== mpsShowDatabaseInfo command =======================================================*/

static int mpsShowDatabaseInfo() {
  pCNDriver->showDatabaseInfo();
  return 0;
}

/*=== mpsShowEngineInfo command =======================================================*/

static int mpsShowEngineInfo() {
  pCNDriver->showEngineInfo();
  return 0;
}

/*=== mpsShowUpdateBuffer command =======================================================*/

static void mpsShowUpdateBuffer(int id) {

  if (id < 0) {
    std::cout << "*** mps show update command ***" << std::endl
	      << "Usage: mps show update <id>" << std::endl
	      << "  id: database Id for the application card (*not* the global id)" << std::endl
	      << "      use the 'mpsg2d'(REVIEW) command to find the database id based on" << std::endl
	      << "      the global id" << std::endl
	      << std::endl
	      << "Prints out the current software update buffer (bits) read from firmware. This" << std::endl
	      << "buffer is updated at 360Hz. The update buffer contents are sets of 'was high' and" << std::endl
	      << "'was low' bits. If all bits are zero then the central node is not receiving any" << std::endl
	      << "update messages from the link node configured with the specified application card." << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  if (id == 999) {
    std::cout << "Full update buffer:" << std::endl;
    std::vector<uint8_t> updateBuffer = Engine::getInstance().getCurrentDb()->getFastUpdateBuffer();
    ApplicationUpdateBufferFullBitSet *buf = reinterpret_cast<ApplicationUpdateBufferFullBitSet *>(&updateBuffer);
    std::cout << buf << std::endl;
  }
  else {
    DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
    if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
      std::cout << (*appCard).second->name << " [global id:"
		<< (*appCard).second->globalId << "]:" << std::endl;

      std::cout << "WasLow: " << std::endl;
      std::cout << *(*appCard).second->wasLowBuffer << std::endl;
      std::cout << "WasHigh: " << std::endl;
      std::cout << *(*appCard).second->wasHighBuffer << std::endl;
    }
  }
}

/*=== mpsShowConfigBuffer command =======================================================*/

static void mpsShowConfigBuffer(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowConfigBuffer or mpsscb command ***" << std::endl
	      << "Usage: mpsscb <id>" << std::endl
	      << "  id: database Id for the application card (*not* the global id)" << std::endl
	      << "      use the 'mpsg2d' command to find the database id based on" << std::endl
	      << "      the global id" << std::endl
	      << std::endl
	      << "Prints out the configuration buffer (bits) for the given application card." << std::endl
	      << "These are the last contents written to the firmware configuration buffere," << std::endl
	      << "where bit 0 is located on the rightmost position." << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
  if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*appCard).second->name << " [global id:"
	      << (*appCard).second->globalId << "]:" << std::endl;
    std::cout << *(*appCard).second->applicationConfigBuffer << std::endl;
    //    (*appCard).second->printAnalogConfiguration();
  }
}

/*=== mpsShowDigitalDevice command =======================================================*/

static void mpsShowDigitalDevice(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowDigitalDevice or mpssdd command ***" << std::endl
	      << "Usage: mpssdd <id>" << std::endl
	      << "  id: database Id for the digital device" << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbDigitalDeviceMap::iterator device = Engine::getInstance().getCurrentDb()->digitalDevices->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->digitalDevices->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowDeviceInput command =======================================================*/

static void mpsShowDeviceInput(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowDeviceInput or mpssdi command ***" << std::endl
	      << "Usage: mpssdi <id>" << std::endl
	      << "  id: database Id for the device input" << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbDeviceInputMap::iterator device = Engine::getInstance().getCurrentDb()->deviceInputs->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowDigitalChannel command =======================================================*/

static void mpsShowDigitalChannel(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowDigitalChannel or mpssdc command ***" << std::endl
	      << "Usage: mpssdc <id>" << std::endl
	      << "  id: database Id for the digital channel" << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbChannelMap::iterator channel = Engine::getInstance().getCurrentDb()->digitalChannels->find(id);
    
  if (channel != Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
    std::cout << (*channel).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowAppCard command =======================================================*/

static void mpsShowAppCard(int id) {
  if (id < 0) {
    std::cout << "*** mps show app [id] ***" << std::endl
	      << "  id: database Id for the application card - *not* the global app id!" << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbApplicationCardMap::iterator card = Engine::getInstance().getCurrentDb()->applicationCards->find(id);
    
  if (card != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*card).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowAnalogDevice command =======================================================*/

static void mpsShowAnalogDevice(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowAnalogDevice or mpssad command ***" << std::endl
	      << "Usage: mpssad <id>" << std::endl
	      << "  id: database Id for the analog device" << std::endl
	      << std::endl;
    return;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();
  DbAnalogDeviceMap::iterator device = Engine::getInstance().getCurrentDb()->analogDevices->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->analogDevices->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsShowMitigationDevice command =======================================================*/

static void mpsShowMitigationDevice(int id) {
  if (id < 0) {
    std::cout << "*** mpsShowMitigationDevice or mpssmd command ***" << std::endl
	      << "Usage: mpssmd <id>" << std::endl
	      << "  id: database Id for the mitigation device" << std::endl
	      << std::endl;
  }

  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }
  
  Engine::getInstance().getCurrentDb()->lock();
  if (id < 0) {
    std::cout << "This are the available mitigation devices: " << std::endl;
    for (DbBeamDestinationMap::iterator it = Engine::getInstance().getCurrentDb()->beamDestinations->begin();
	 it != Engine::getInstance().getCurrentDb()->beamDestinations->end(); ++it) {
      std::cout << "  [" << (*it).second->id << "] " << (*it).second->name << std::endl;
    }
    std::cout << std::endl;
    return;
  }
  
  DbBeamDestinationMap::iterator device = Engine::getInstance().getCurrentDb()->beamDestinations->find(id);
    
  if (device != Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
    std::cout << (*device).second << std::endl;
  }
  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsEnableApp command =======================================================*/

static void mpsEnableApp(int id, int enableInt) {
  if (id < 0) {
    std::cout << "*** mpsEnableApp or mpsea command ***" << std::endl
	      << "Usage: mpsea <globalId> <enable>" << std::endl
	      << "  globalId: unique application card id (0 to 1023)" << std::endl
	      << "  enable: 0 to disable, 1 to enable timeouts" << std::endl
	      << "" << std::endl
	      << "This command clears/sets the timeout enable bit for the"
	      << "specified application card." << std::endl;
    return;
  } 
  
  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();

  bool enable = false;
  if (enableInt > 0) {
    enable = true;
  }
  Firmware::getInstance().setAppTimeoutEnable(id, enable);
  Firmware::getInstance().writeAppTimeoutMask();

  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mpsApp2Db command =======================================================*/

static void mpsApp2Db(int id) {
  if (id < 0) {
    std::cout << "Usage: mpsg2d <globalId>" << std::endl
	      << "  globalId: unique application card id (0 to 1023)" << std::endl
	      << "" << std::endl
	      << "Searches the database for the application card information " << std::endl
	      << "for the specified global id, and prints out the application" << std::endl
	      << "card database id and its attributes." << std::endl;
    return;
  }
  
  if (!Engine::getInstance().getCurrentDb()) {
    std::cerr << "ERROR: No database loaded" << std::endl;
    return;
  }

  Engine::getInstance().getCurrentDb()->lock();

  bool found = false;
  DbApplicationCardPtr appCard;
  uint32_t dbId = 0;
  for (DbApplicationCardMap::iterator it = Engine::getInstance().getCurrentDb()->applicationCards->begin();
       it != Engine::getInstance().getCurrentDb()->applicationCards->end(); ++it) {
    if ((*it).second->globalId == id) {
      found = true;
      dbId = (*it).second->id;
      appCard = (*it).second;
    }
  }

  if (found) {
    std::cout << "DatabaseId: " << dbId << std::endl;
    std::cout << appCard << std::endl;
  }
  else {
    std::cerr << "ERROR: no entry found for globalId=" << id << std::endl;
  }

  Engine::getInstance().getCurrentDb()->unlock();
}

/*=== mps command =======================================================*/

static void printHelp() {
    std::cout << "Usage: mps [command] [arg0] [arg1] [arg2]" << std::endl
	      << "  help                   : print this help" << std::endl
	      << "  app db [id]            : convert app dd to database id" << std::endl
	      << "  print bypass           : print bypass queue" << std::endl
	      << "  enable app [id] [en]   : id=appId, en=1 enable, en=0 disable" << std::endl
	      << "  show" << std::endl
	      << "  |- show database       : print database info" << std::endl
	      << "  |- show engine         : print slow engine info" << std::endl
	      << "  |- show firmware       : print firmware info" << std::endl
	      << "  |- show destination    : print beam destination info" << std::endl
	      << "  |- show faults         : print current MPS faults" << std::endl
	      << "  |- show app [id]       : print application card info" << std::endl
    	      << "  |- show digital [id]   : print digital device info" << std::endl
	      << "  |- show input [id]     : print device input info (for digital devices)" << std::endl
	      << "  |- show channel [id]   : print channel info (for digital devices)" << std::endl
	      << "  |- show analog [id]    : show analog device info" << std::endl
	      << "  |- show config [id]    : print fw configuration buffer for app" << std::endl
	      << "  |- show update [id]    : print latest MPS update message for app" << std::endl
	      << "  |- show fault [id]     : print fault info" << std::endl
	      << "  |- show mitigation [id]: print mitigation device info" << std::endl
	      << "" << std::endl
	      << "*** The id specified to the mps command is the database id   ***" << std::endl
	      << "*** Use id=-1 for additional help (e.g. 'mps show fault -1') ***" << std::endl;
}

static const iocshArg mpsArg0 = {"[help|show]", iocshArgString};
static const iocshArg mpsArg1 = {"[option]", iocshArgString};
static const iocshArg mpsArg2 = {"[id]", iocshArgInt};
static const iocshArg mpsArg3 = {"[arg3]", iocshArgInt};
static const iocshArg * const mpsArgs[4] = {&mpsArg0, &mpsArg1, &mpsArg2, &mpsArg3};
static const iocshFuncDef mpsFuncDef = {"mps", 4, mpsArgs};
static void mpsCallFunc(const iocshArgBuf *args) {
  if (args[0].sval == NULL) {
    printHelp();
    return;
  }

  std::string command(args[0].sval);

  if (command == "help") {
    std::string option = "all";
    if (args[1].sval != NULL) {
      option = args[1].sval;
    }
    printHelp();
    return;
  }

  if (command == "app") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "db") {
      int appId = args[2].ival;
      mpsApp2Db(appId);
    }
  }
  else if (command == "print") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "bypass") {
      mpsPrintQueue();
    }
  }
  else if (command == "enable") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "app") {
      int id = args[2].ival;
      int enable = args[3].ival;
      mpsEnableApp(id, enable);
    }
  }
  else if (command == "show" || command == "s") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "db" || option == "database") {
      mpsShowDatabaseInfo();
    }
    else if (option == "engine" || option == "en") {
      mpsShowEngineInfo();
    }
    else if (option == "fw" || option == "firmware") {
      mpsShowFirmware();
    }
    else if (option == "dest" || option == "destination") {
      mpsShowMitigation();
    }
    else if (option == "faults") {
      mpsShowFaults();
    }
    else if (option == "mit" || option == "mitigation") {
      int id = args[2].ival;
      mpsShowMitigationDevice(id);
    }
    else if (option == "app") {
      int id = args[2].ival;
      mpsShowAppCard(id);
    }
    else if (option == "ad" || option == "analog") {
      int id = args[2].ival;
      mpsShowAnalogDevice(id);
    }
    else if (option == "cb" || option == "config") {
      int id = args[2].ival;
      mpsShowConfigBuffer(id);
    }
    else if (option == "ub" || option == "update") {
      int id = args[2].ival;
      mpsShowUpdateBuffer(id);
    }
    else if (option == "di" || option == "input") {
      int id = args[2].ival;
      mpsShowDeviceInput(id);
    }
    else if (option == "dc" || option == "channel") {
      int id = args[2].ival;
      mpsShowDigitalChannel(id);
    }
    else if (option == "dd" || option == "digital") {
      int id = args[2].ival;
      mpsShowDigitalDevice(id);
    }
    else if (option == "fault") {
      int id = args[2].ival;
      mpsShowFault(id);
    }
    else {
      std::cout << "ERROR: unknown option \"" << option << "\"" << std::endl;
      return;
    }
  }
}

static void mpsRegistrar(void) {
  iocshRegister(&mpsFuncDef, mpsCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsRegistrar);
}
