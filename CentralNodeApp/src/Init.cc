#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <mutex>

#include <iocsh.h>
#include <epicsThread.h>
#include <initHooks.h>
#include <yamlLoader.h>

#include "CentralNodeDriver.h"
#include "Log.h"

#include "central_node_database_tables.h"
#include "central_node_database.h"
#include "central_node_engine.h"

#ifdef BOOST_TEST
  #define BOOST_TEST_MODULE CENTRAL_NODE_TESTS
  #define BOOST_TEST_NO_MAIN
  #define BOOST_TEST_ALTERNATIVE_INIT_API
  #include <boost/test/included/unit_test.hpp> 
#endif

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

static void mpsShowUpdateBuffer(int32_t id) {

  if (id < 0) {
    std::cout << "*** mps show update or mps s ub command ***" << std::endl
	      << "Usage: mps show update <id>" << std::endl
	      << "  id: database Id for the application card (*not* the number)" << std::endl
	      << "      use the 'mpsg2d'(REVIEW) command to find the database id based on" << std::endl
	      << "      the number" << std::endl
	      << std::endl
	      << "Prints out the current software update buffer (bits) read from firmware. This" << std::endl
	      << "buffer is updated at 360Hz. The update buffer contents are sets of 'was high' and" << std::endl
	      << "'was low' bits. If all bits are zero then the central node is not receiving any" << std::endl
	      << "update messages from the link node configured with the specified application card." << std::endl
	      << std::endl;
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
      std::cout << (*appCard).second->applicationType->name << " [number:"
		<< (*appCard).second->number << "]:" << std::endl;

      std::cout << "WasLow: " << std::endl;
      std::cout << *(*appCard).second->getWasLowBuffer() << std::endl;
      std::cout << "WasHigh: " << std::endl;
      std::cout << *(*appCard).second->getWasHighBuffer() << std::endl;
    }
  }
}

/*=== mpsShowConfigBuffer command =======================================================*/

static void mpsShowConfigBuffer(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show config or mps s cb command ***" << std::endl
	      << "Usage: mps show config <id>" << std::endl
	      << "  id: database Id for the application card (*not* the number)" << std::endl
	      << "      use the 'mpsg2d' command to find the database id based on" << std::endl
	      << "      the number" << std::endl
	      << std::endl
	      << "Prints out the configuration buffer (bits) for the given application card." << std::endl
	      << "These are the last contents written to the firmware configuration buffere," << std::endl
	      << "where bit 0 is located on the rightmost position." << std::endl
	      << std::endl;
    return;
  }
  DbApplicationCardMap::iterator appCard = Engine::getInstance().getCurrentDb()->applicationCards->find(id);

  if (appCard != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*appCard).second->applicationType->name << " [number:"
	      << (*appCard).second->number << "]:" << std::endl;
    std::cout << *(*appCard).second->applicationConfigBuffer << std::endl;
    //    (*appCard).second->printAnalogConfiguration();
  }
}

/*=== mpsShowFaultInput command =======================================================*/

static void mpsShowFaultInput(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show input or mps s fi command ***" << std::endl
	      << "Usage: mps show input <id>" << std::endl
	      << "  id: database Id for the fault input" << std::endl
	      << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  DbFaultInputMap::iterator fault = Engine::getInstance().getCurrentDb()->faultInputs->find(id);

  if (fault != Engine::getInstance().getCurrentDb()->faultInputs->end()) {
    std::cout << (*fault).second << std::endl;
  }
}

/*=== mpsShowDigitalChannel command =======================================================*/

static void mpsShowDigitalChannel(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show digital or mps s dc command ***" << std::endl
	      << "Usage: mps show digital <id>" << std::endl
	      << "  id: database Id for the digital channel" << std::endl
	      << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  DbDigitalChannelMap::iterator channel = Engine::getInstance().getCurrentDb()->digitalChannels->find(id);

  if (channel != Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
    std::cout << (*channel).second << std::endl;
  }
}

/*=== mpsShowAppCard command =======================================================*/

static void mpsShowAppCard(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show app or mps s a command ***" << std::endl
        << "Usage: mps show app <id>" << std::endl
	      << "  id: database Id for the application card - *not* the number!" << std::endl
	      << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  DbApplicationCardMap::iterator card = Engine::getInstance().getCurrentDb()->applicationCards->find(id);

  if (card != Engine::getInstance().getCurrentDb()->applicationCards->end()) {
    std::cout << (*card).second << std::endl;
  }
}

/*=== mpsShowAnalogChannel command =======================================================*/

static void mpsShowAnalogChannel(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show analog or mps s ac command ***" << std::endl
	      << "Usage: mps show analog <id>" << std::endl
	      << "  id: database Id for the analog channel" << std::endl
	      << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  DbAnalogChannelMap::iterator channel = Engine::getInstance().getCurrentDb()->analogChannels->find(id);

  if (channel != Engine::getInstance().getCurrentDb()->analogChannels->end()) {
    std::cout << (*channel).second << std::endl;
  }
}

/*=== mpsShowMitigationDevice command =======================================================*/

static void mpsShowMitigationDevice(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show mitigation or mps s mit command ***" << std::endl
	      << "Usage: mps show mitigation <id>" << std::endl
	      << "  id: database Id for the mitigation device" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  if (id < 0) {
    std::cout << "These are the available mitigation devices: " << std::endl;
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
}

/*=== mpsShowAllowedClass (mitigation) command =======================================================*/

static void mpsShowAllowedClass(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show class or mps s cls command ***" << std::endl
	      << "Usage: mps show class <id>" << std::endl
	      << "  id: database Id for the allowed class (mitigation)" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  DbAllowedClassMap::iterator it = Engine::getInstance().getCurrentDb()->allowedClasses->find(id);

  if (it != Engine::getInstance().getCurrentDb()->allowedClasses->end()) {
    std::cout << (*it).second << std::endl;
  }
}

/*=== mpsShowFault command =======================================================*/
static void mpsShowFault(int32_t id) {  
  if (id < 0) {
    std::cout << "*** mps show fault or mps s f command ***" << std::endl
	      << "Usage: mps show fault <id>" << std::endl
	      << "  id: database Id for the fault" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  DbFaultMap::iterator fault = Engine::getInstance().getCurrentDb()->faults->find(id);

  if (fault != Engine::getInstance().getCurrentDb()->faults->end()) {
    std::cout << (*fault).second << std::endl;
  }
}

/*=== mpsShowFaultState command =======================================================*/

static void mpsShowFaultState(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show state or mps s s command ***" << std::endl
	      << "Usage: mps show state <id>" << std::endl
	      << "  id: database Id for the fault state" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  DbFaultStateMap::iterator faultState = Engine::getInstance().getCurrentDb()->faultStates->find(id);

  if (faultState != Engine::getInstance().getCurrentDb()->faultStates->end()) {
    std::cout << (*faultState).second << std::endl;
  }
}

/*=== mpsShowLinkNode command =======================================================*/

static void mpsShowLinkNode(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show node or mps s n command ***" << std::endl
	      << "Usage: mps show node <id>" << std::endl
	      << "  id: database Id for the link node" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  DbLinkNodeMap::iterator linkNode = Engine::getInstance().getCurrentDb()->linkNodes->find(id);

  if (linkNode != Engine::getInstance().getCurrentDb()->linkNodes->end()) {
    std::cout << (*linkNode).second << std::endl;
  }
}

/*=== mpsShowCrate command =======================================================*/

static void mpsShowCrate(int32_t id) {
  if (id < 0) {
    std::cout << "*** mps show crate or mps s c command ***" << std::endl
	      << "Usage: mps s c <id>" << std::endl
	      << "  id: database Id for the crate" << std::endl
	      << std::endl;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  DbCrateMap::iterator crate = Engine::getInstance().getCurrentDb()->crates->find(id);

  if (crate != Engine::getInstance().getCurrentDb()->crates->end()) {
    std::cout << (*crate).second << std::endl;
  }
}

/*=== mpsShowTestMode command ==================================================*/

static void mpsShowTestMode() {
  std::string mode = "All Apps Disabled - Test Mode";
  if (pCNDriver->getTestMode()) {
    mode = "Normal Operation";
  }
  std::cout << "Test Mode Flag: " << mode << std::endl;
}

/*=== mpsEnableApp command =======================================================*/

static void mpsEnableApp(int32_t id, int enableInt) {
  if (id < 0) {
    std::cout << "*** mps enable app or mps e a command ***" << std::endl
	      << "Usage: mps enable app <number> <enable>" << std::endl
	      << "  number: unique application card id (0 to 1023)" << std::endl
	      << "  enable: 0 to disable, 1 to enable timeouts" << std::endl
	      << "" << std::endl
	      << "This command clears/sets the timeout enable bit for the"
	      << "specified application card." << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  bool enable = false;
  if (enableInt > 0) {
    enable = true;
  }
  Firmware::getInstance().setAppTimeoutEnable(id, enable);
  Firmware::getInstance().writeAppTimeoutMask();
}

/*=== mpsEnableAllApp command =======================================================*/

static void mpsEnableAllApp(int enableInt) {
  if (enableInt < 0) {
    std::cout << "*** mps enable app or mps e a command ***" << std::endl
	      << "Usage: mps enable app <enable>" << std::endl
	      << "  enable: 0 to disable, 1 to enable timeouts" << std::endl
	      << "" << std::endl
	      << "This command clears/sets the timeout enable bit for all"
	      << "the application cards in this central node." << std::endl;
    return;
  }  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  bool enable = false;
  if (enableInt < 1) {
    pCNDriver->setTestMode(0);
  }
  else {
    pCNDriver->setTestMode(1);
  }
  uint32_t id;
  DbApplicationCardMap::iterator applicationCardIt;
  for (applicationCardIt = Engine::getInstance().getCurrentDb()->applicationCards->begin();
       applicationCardIt != Engine::getInstance().getCurrentDb()->applicationCards->end();
       ++applicationCardIt) {
          if (enableInt > 0) {
            enable = (*applicationCardIt).second->hasInputs;
          }
          id = (*applicationCardIt).second->number;
          Firmware::getInstance().setAppTimeoutEnable(id, enable);
          Firmware::getInstance().writeAppTimeoutMask();
  }
  mpsShowTestMode();
}

/*=== mpsApp2Db command =======================================================*/

static void mpsApp2Db(int32_t id) {
  if (id < 0) {
    std::cout << "Usage: mps app db <number>" << std::endl
        << "Usage: mps app db <number>" << std::endl
	      << "  number: unique application card id (0 to 1023)" << std::endl
	      << "" << std::endl
	      << "Searches the database for the application card information " << std::endl
	      << "for the specified number, and prints out the application" << std::endl
	      << "card database id and its attributes." << std::endl;
    return;
  }
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

  bool found = false;
  DbApplicationCardPtr appCard;
  uint32_t dbId = 0;
  for (DbApplicationCardMap::iterator it = Engine::getInstance().getCurrentDb()->applicationCards->begin();
       it != Engine::getInstance().getCurrentDb()->applicationCards->end(); ++it) {
    if ((*it).second->number == (uint32_t)id) {
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
    std::cerr << "ERROR: no entry found for number=" << id << std::endl;
  }
}

/*=== mpsSetPCStreamDebug command ================================================*/

static void mpsSetPCStreamDebug(int enableInt) {
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  bool enable = false;
  if (enableInt > 0) {
    enable = true;
  }
  Engine::getInstance().getCurrentDb()->PCChangeSetDebug(enable);
}

/*=== mpsPrintPCCounter command ==================================================*/

static void mpsPrintPCCounter() {
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  Engine::getInstance().getCurrentDb()->printPCCounters();
}

/*=== mpsPrintTables command ==================================================*/

static void mpsPrintTables() {
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  std::cout << Engine::getInstance().getCurrentDb() << std::endl;
}

/*=== mpsClearLatch command ==================================================*/

static void mpsClearLatch() {
  if ( Engine::getInstance().unlatchAllowed() ){
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      Engine::getInstance().getCurrentDb()->unlatchAll(); // Clear all software latches (digital/analog)
    }
    Engine::getInstance().clearSoftwareLatch();
    Firmware::getInstance().evalLatchClear(); // Clear the latches within the firmware
  }
  Engine::getInstance().startLatchTimeout();
}

/*=== mpsBypassfault command =======================================================*/

static void mpsBypassFault(int id, int stateId, epicsInt32 expirationTime) {
  if (id <= 0) {
    std::cout << "*** mps bypass fault or mps b f command ***" << std::endl
	      << "Usage: mps b f <id> <fault state id> <time (sec)>" << std::endl
	      << "  id: database Id for the fault" << std::endl
        << "  if analog fault: mps b f <id> 0 <time (sec)>" << std::endl
	      << std::endl;
    return;
  }
  // Add expirationTime to current time, unless the bypass is being cancelled
  time_t now;
  time(&now);
  if (expirationTime > 0) {
    expirationTime += now;
  }
  else {
    expirationTime = 0;
  }
  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    Engine::getInstance().getBypassManager()->bypassFault(id, stateId, expirationTime);
  }
}

/*=== mpsBypassApp command =======================================================*/

static void mpsBypassApp(int id, epicsInt32 expirationTime) {
  if (id <= 0) {
    std::cout << "*** mps bypass app or mps b a command ***" << std::endl
	      << "Usage: mps b a <id> <time (sec)>" << std::endl
	      << "  id: database Id for the application card" << std::endl
	      << std::endl;
    return;
  }
  // Add expirationTime to current time, unless the bypass is being cancelled
  time_t now;
  time(&now);
  if (expirationTime > 0) {
    expirationTime += now;
  }
  else {
    expirationTime = 0;
  }
  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    Engine::getInstance().getBypassManager()->setBypass(BYPASS_APPLICATION, id, 0, expirationTime);
  }
}

/*=== mps command =======================================================*/

static void printHelp() {
    std::cout << "Usage: mps [command] [arg0] [arg1] [arg2]" << std::endl
	      << "  help                   : print this help" << std::endl
	      << "  app db [id]            : convert app dd to database id" << std::endl
	      << "  print bypass           : print bypass queue" << std::endl
	      << "  enable app [id] [en]   : id=appId, use 1024 for all, en=1 enable, en=0 disable" << std::endl
          << "  debug" << std::endl
          << "  |- debug pcstream [en] : Power class stream debug, en=1 enable, en=0 disable" << std::endl
	      << "  show" << std::endl
	      << "  |- show database       : print database info" << std::endl
	      << "  |- show engine         : print slow engine info" << std::endl
	      << "  |- show firmware       : print firmware info" << std::endl
	      << "  |- show destination    : print beam destination info" << std::endl
	      << "  |- show faults         : print current MPS faults" << std::endl
	      << "  |- show app [id]       : print application card info" << std::endl
        << "  |- show digital [id]   : print digital channel info" << std::endl
	      << "  |- show input [id]     : print fault input info" << std::endl
	      << "  |- show analog [id]    : print analog channel info" << std::endl
	      << "  |- show config [id]    : print fw configuration buffer for app" << std::endl
	      << "  |- show update [id]    : print latest MPS update message for app" << std::endl
	      << "  |- show fault [id]     : print fault info" << std::endl
	      << "  |- show mitigation [id]: print mitigation device info" << std::endl
        << "  |- show class [id]     : print mitigation (allowed class) info" << std::endl
        << "  |- show state [id]     : print fault state info" << std::endl
        << "  |- show node [id]      : print link node info" << std::endl
        << "  |- show crate [id]     : print crate info" << std::endl
        << "  |- show test           : print All App Disable Flag (test mode)" << std::endl
        << "  |- show pccounters     : print power class change counters" << std::endl
        << "  |- show tables         : print All the records of the database" << std::endl
        << "  clear" << std::endl
        << "  |- clear beam          : clear beam fault" << std::endl
        << "  |- clear latch         : clear evaluation latch" << std::endl
        << "  |- clear mon           : clear monitor error" << std::endl
        << "  |- clear sw            : clear software error" << std::endl
        << "  |- clear mo            : clear mitigation message concentrator error" << std::endl
        << "  |- clear to            : clear timeout error" << std::endl
        << "  bypass" << std::endl
        << "  |- bypass fault [id] [state_id] [time] : bypass fault to fault state" << std::endl
        << "  |- bypass app [id] [time] : bypass application card (Disable timeout)" << std::endl
	      << "" << std::endl
	      << "*** The id specified to the mps command is the database id   ***" << std::endl
	      << "*** Use id=-1 for additional help (e.g. 'mps show fault -1') ***" << std::endl;
}

static const iocshArg mpsArg0 = {"[help|show]", iocshArgString};
static const iocshArg mpsArg1 = {"[option]", iocshArgString};
static const iocshArg mpsArg2 = {"[id]", iocshArgInt};
static const iocshArg mpsArg3 = {"[arg3]", iocshArgInt};
static const iocshArg mpsArg4 = {"[arg4]", iocshArgInt};
static const iocshArg * const mpsArgs[5] = {&mpsArg0, &mpsArg1, &mpsArg2, &mpsArg3, &mpsArg4};
static const iocshFuncDef mpsFuncDef = {"mps", 5, mpsArgs};
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
      int32_t appId = args[2].ival;
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
    if (option == "app" || option == "a") {
      int32_t id = args[2].ival;
      int enable = args[3].ival;
      if (id > 1023) {
        mpsEnableAllApp(enable);
      } else {
        mpsEnableApp(id, enable);
      }
    }
  }

  else if (command == "show" || command == "s") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }

    if (!Engine::getInstance().getCurrentDb()) {
      std::cerr << "ERROR: No database loaded" << std::endl;
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
      int32_t id = args[2].ival;
      mpsShowMitigationDevice(id);
    }
    else if (option == "cls" || option == "class") {
      int32_t id = args[2].ival;
      mpsShowAllowedClass(id);
    }
    else if (option == "a" || option == "app") {
      int32_t id = args[2].ival;
      mpsShowAppCard(id);
    }
    else if (option == "ac" || option == "analog") {
      int32_t id = args[2].ival;
      mpsShowAnalogChannel(id);
    }
    else if (option == "cb" || option == "config") {
      int32_t id = args[2].ival;
      mpsShowConfigBuffer(id);
    }
    else if (option == "ub" || option == "update") {
      int32_t id = args[2].ival;
      mpsShowUpdateBuffer(id);
    }
    else if (option == "fi" || option == "input") {
      int32_t id = args[2].ival;
      mpsShowFaultInput(id);
    }
    else if (option == "dc" || option == "digital") {
      int32_t id = args[2].ival;
      mpsShowDigitalChannel(id);
    }
    else if (option == "s" || option == "state") {
      int32_t id = args[2].ival;
      mpsShowFaultState(id);
    }
    else if (option == "n" || option == "node") {
      int32_t id = args[2].ival;
      mpsShowLinkNode(id);
    }
    else if (option == "c" || option == "crate") {
      int32_t id = args[2].ival;
      mpsShowCrate(id);
    }
    else if (option == "fault" || option == "f") {
      int32_t id = args[2].ival;
      mpsShowFault(id);
    }
    else if (option == "test") {
      mpsShowTestMode();
    }
    else if (option == "pccounters") {
      mpsPrintPCCounter();
    }
    else if (option == "tables") {
      mpsPrintTables();
    }
    else {
      std::cout << "ERROR: unknown option \"" << option << "\"" << std::endl;
      return;
    }
  }
  else if (command == "debug") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "pcstream") {
        int enable = args[2].ival;
        mpsSetPCStreamDebug(enable);
    }
    else {
      std::cout << "ERROR: unknown option \"" << option << "\"" << std::endl;
      return;
    }
  }

  else if (command == "clear") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "beam") {
      Firmware::getInstance().beamFaultClear(); 
    }
    else if (option == "latch") {
      mpsClearLatch();
    }
    else if (option == "mon") {
      Firmware::getInstance().monErrClear();
    }
    else if (option == "sw") {
      Firmware::getInstance().swErrClear();
    }
    else if (option == "mo") {
      Firmware::getInstance().moConcErrClear();
    }
    else if (option == "to") {
      Firmware::getInstance().toErrClear();
    }
    else {
      std::cout << "ERROR: unknown option \"" << option << "\"" << std::endl;
      return;
    }
  }

  else if (command == "bypass" || command == "b") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "fault" || option == "f") {
      int32_t id = args[2].ival;
      int32_t faultStateId = args[3].ival;
      uint32_t expTime = args[4].ival;
      mpsBypassFault(id, faultStateId, expTime); 
    }
    else if (option == "app" || option == "a") {
      int32_t id = args[2].ival;
      uint32_t expTime = args[3].ival;
      mpsBypassApp(id, expTime); 
    }
  }
  #ifdef BOOST_TEST
  else if (command == "test" || command == "t") {
    if (args[1].sval == NULL) {
      std::cout << "ERROR: missing option" << std::endl;
      printHelp();
      return;
    }
    std::string option(args[1].sval);
    if (option == "bypass" || option == "b") {
      char *argv[] = { (char*)"CentralNode", (char*)"--report_level=short", (char*)"--show_progress", 
                    (char*)"--logger=HRF,error,stdout", (char*)"--run_test=BYPASS_TESTS", NULL };
      boost::unit_test::unit_test_main(init_unit_test, 5, argv);
    }
  }
  #endif

}

static void mpsRegistrar(void) {
  iocshRegister(&mpsFuncDef, mpsCallFunc);
}

extern "C" {
  epicsExportRegistrar(mpsRegistrar);
}
