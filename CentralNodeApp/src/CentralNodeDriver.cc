#include "CentralNodeDriver.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <mutex>

#include <epicsTypes.h>
#include <epicsExport.h>
#include <drvSup.h>
#include <dbCommon.h>
#include <alarm.h>

#include "LogWrapper.h"

#include <central_node_engine.h>
#include <central_node_bypass.h>

#if defined(LOG_ENABLED) && !defined(LOG_STDOUT)
using namespace easyloggingpp;
static Logger *centralNodeLogger;
#endif

CentralNodeDriver::CentralNodeDriver(const char *portName, std::string configPath,
				     std::string historyServer, int historyPort) :
  asynPortDriver(portName, 100, CENTRAL_NODE_DRIVER_NUM_PARAMS,
		 asynOctetMask | asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask | asynUInt32DigitalMask | asynDrvUserMask, // interfaceMask
		 asynInt32Mask | asynInt16ArrayMask | asynInt8ArrayMask, // interruptMask
		 ASYN_MULTIDEVICE,                       // asynFlags
		 1, 0, 0),            // autoConnect, priority, stackSize
  _configPath(configPath) {
#if defined(LOG_ENABLED) && !defined(LOG_STDOUT)
  centralNodeLogger = Loggers::getLogger("DRIVER");
#endif
  LOG_TRACE("DRIVER", "CentralNodeDriver constructor");

  char *inputUpdateTimeoutStr = getenv(MPS_ENV_UPDATE_TIMEOUT);
  if (inputUpdateTimeoutStr == NULL) {
    _inputUpdateTimeout = 3500;
  }
  else {
    _inputUpdateTimeout = atoi(inputUpdateTimeoutStr);
  }

  createParam(MPS_CONFIG_LOAD_STRING, asynParamOctet, &_mpsConfigLoadParam);
  createParam(MPS_DEVICE_INPUT_STRING, asynParamUInt32Digital, &_mpsDeviceInputParam);
  createParam(MPS_ANALOG_DEVICE_STRING, asynParamUInt32Digital, &_mpsAnalogDeviceParam);
  createParam(MPS_SW_MITIGATION_STRING, asynParamInt32, &_mpsSwMitigationParam);
  createParam(MPS_FAULT_STRING, asynParamUInt32Digital, &_mpsFaultParam);
  createParam(MPS_FAULT_IGNORED_STRING, asynParamUInt32Digital, &_mpsFaultIgnoredParam);
  createParam(MPS_FAULT_LATCHED_STRING, asynParamUInt32Digital, &_mpsFaultLatchedParam);
  createParam(MPS_FAULT_UNLATCH_STRING, asynParamUInt32Digital, &_mpsFaultUnlatchParam);
  createParam(MPS_DEVICE_INPUT_LATCHED_STRING, asynParamUInt32Digital, &_mpsDeviceInputLatchedParam);
  createParam(MPS_DEVICE_INPUT_UNLATCH_STRING, asynParamUInt32Digital, &_mpsDeviceInputUnlatchParam);
  createParam(MPS_DEVICE_INPUT_BYPV_STRING, asynParamUInt32Digital, &_mpsDeviceInputBypassValueParam);
  createParam(MPS_DEVICE_INPUT_BYPS_STRING, asynParamUInt32Digital, &_mpsDeviceInputBypassStatusParam);
  createParam(MPS_DEVICE_INPUT_BYPEXPDATE_STRING, asynParamInt32, &_mpsDeviceInputBypassExpirationDateParam);
  createParam(MPS_DEVICE_INPUT_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsDeviceInputBypassRemainingExpirationTimeParam);
  createParam(MPS_DEVICE_INPUT_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsDeviceInputBypassExpirationDateStringParam);
  createParam(MPS_ANALOG_DEVICE_LATCHED_STRING, asynParamUInt32Digital, &_mpsAnalogDeviceLatchedParam);
  createParam(MPS_ANALOG_DEVICE_UNLATCH_STRING, asynParamUInt32Digital, &_mpsAnalogDeviceUnlatchParam);
  createParam(MPS_ANALOG_DEVICE_BYPV_STRING, asynParamUInt32Digital, &_mpsAnalogDeviceBypassValueParam);
  createParam(MPS_ANALOG_DEVICE_BYPS_STRING, asynParamInt32, &_mpsAnalogDeviceBypassStatusParam);
  createParam(MPS_ANALOG_DEVICE_BYPEXPDATE_STRING, asynParamInt32, &_mpsAnalogDeviceBypassExpirationDateParam);
  createParam(MPS_ANALOG_DEVICE_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsAnalogDeviceBypassRemainingExpirationTimeParam);
  createParam(MPS_ANALOG_DEVICE_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsAnalogDeviceBypassExpirationDateStringParam);
  createParam(MPS_ANALOG_DEVICE_IGNORED_STRING, asynParamUInt32Digital, &_mpsAnalogDeviceIgnoredParam);
  createParam(MPS_UNLATCH_ALL_STRING, asynParamInt32, &_mpsUnlatchAllParam);
  createParam(MPS_FW_BUILD_STAMP_STRING_STRING, asynParamOctet, &_mpsFwBuildStampParam);
  createParam(MPS_ENABLE_STRING, asynParamUInt32Digital, &_mpsEnableParam);
  createParam(MPS_ENABLE_RBV_STRING, asynParamUInt32Digital, &_mpsEnableRbvParam);
  createParam(MPS_SW_ENABLE_STRING, asynParamUInt32Digital, &_mpsSwEnableParam);
  createParam(MPS_SW_ENABLE_RBV_STRING, asynParamUInt32Digital, &_mpsSwEnableRbvParam);
  createParam(MPS_FAULT_REASON_STRING, asynParamInt32, &_mpsFaultReasonParam);
  createParam(MPS_SW_UPDATE_RATE_STRING, asynParamInt32, &_mpsUpdateRateParam);
  createParam(MPS_TIMING_CHECK_ENABLE_STRING, asynParamUInt32Digital, &_mpsTimingCheckEnableParam);
  createParam(MPS_TIMING_CHECK_ENABLE_RBV_STRING, asynParamUInt32Digital, &_mpsTimingCheckEnableRbvParam);
  createParam(MPS_FW_MITIGATION_STRING, asynParamInt32, &_mpsFwMitigationParam);
  createParam(MPS_MITIGATION_STRING, asynParamInt32, &_mpsMitigationParam);
  createParam(MPS_SW_UPDATE_COUNTER_STRING, asynParamInt32, &_mpsUpdateCounterParam);
  createParam(MPS_ENGINE_START_TIME_STRING_STRING, asynParamOctet, &_mpsEngineStartTimeStringParam);
  createParam(MPS_LATCHED_MITIGATION_STRING, asynParamInt32, &_mpsLatchedMitigationParam);
  createParam(MPS_MITIGATION_UNLATCH_STRING, asynParamUInt32Digital, &_mpsMitigationUnlatchParam);
  createParam(MPS_APP_STATUS_STRING, asynParamUInt32Digital, &_mpsAppStatusParam);
  createParam(MPS_APP_TIMESTAMP_LOW_BITS_STRING, asynParamInt32, &_mpsAppTimestampLowBitsParam);
  createParam(MPS_APP_TIMESTAMP_HIGH_BITS_STRING, asynParamInt32, &_mpsAppTimestampHighBitsParam);
  createParam(MPS_EVALUATION_ENABLE_STRING, asynParamUInt32Digital, &_mpsEvaluationEnableParam);
  createParam(MPS_EVALUATION_ENABLE_RBV_STRING, asynParamUInt32Digital, &_mpsEvaluationEnableRbvParam);
  createParam(MPS_MON_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsMonErrClearParam);
  createParam(MPS_SW_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsSwErrClearParam);
  createParam(MPS_TO_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsToErrClearParam);
  createParam(MPS_MO_CONC_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsMoConcErrClearParam);
  createParam(MPS_TIMEOUT_ENABLE_STRING, asynParamUInt32Digital, &_mpsTimeoutEnableParam);
  createParam(MPS_TIMEOUT_ENABLE_RBV_STRING, asynParamUInt32Digital, &_mpsTimeoutEnableRbvParam);
  createParam(MPS_EVALCYCLE_AVG_TIME_STRING, asynParamInt32, &_mpsEvalCycleAvgParam);
  createParam(MPS_EVALCYCLE_MAX_TIME_STRING, asynParamInt32, &_mpsEvalCycleMaxParam);
  createParam(MPS_EVAL_AVG_TIME_STRING, asynParamInt32, &_mpsEvalAvgParam);
  createParam(MPS_EVAL_MAX_TIME_STRING, asynParamInt32, &_mpsEvalMaxParam);
  createParam(MPS_UPDATE_AVG_TIME_STRING, asynParamInt32, &_mpsUpdateAvgParam);
  createParam(MPS_UPDATE_MAX_TIME_STRING, asynParamInt32, &_mpsUpdateMaxParam);
  createParam(MPS_UPDATE_TIME_CLEAR_STRING, asynParamUInt32Digital, &_mpsUpdateClearParam);
  createParam(MPS_FWUPDATDE_AVG_PERIOD_STRING, asynParamInt32, &_mpsFwUpdateAvgParam);
  createParam(MPS_FWUPDATDE_MAX_PERIOD_STRING, asynParamInt32, &_mpsFwUpdateMaxParam);
  createParam(MPS_WDUPDATDE_AVG_PERIOD_STRING, asynParamInt32, &_mpsWdUpdateAvgParam);
  createParam(MPS_WDUPDATDE_MAX_PERIOD_STRING, asynParamInt32, &_mpsWdUpdateMaxParam);
  createParam(MPS_CONFIG_DB_SRC_STRING, asynParamOctet, &_mpsConfigDbSourceParam);
  createParam(MPS_CONFIG_DB_USER_STRING, asynParamOctet, &_mpsConfigDbUserParam);
  createParam(MPS_CONFIG_DB_DATE_STRING, asynParamOctet, &_mpsConfigDbDateParam);
  createParam(MPS_CONFIG_DB_MD5SUM_STRING, asynParamOctet, &_mpsConfigDbMd5SumParam);
  createParam(MPS_STATE_STRING, asynParamInt32, &_mpsStateParam);
  createParam(MPS_CONDITION_STRING, asynParamUInt32Digital, &_mpsConditionParam);
  createParam(MPS_FW_SOFTWARE_WDOG_COUNTER_STRING, asynParamInt32, &_mpsFwSoftwareWdogCounterParam);
  createParam(MPS_FW_MONITOR_NOT_READY_COUNTER_STRING, asynParamInt32, &_mpsFwMonitorNotReadyCounterParam);
  createParam(MPS_SKIP_HEARTBEAT_STRING, asynParamUInt32Digital, &_mpsSkipHeartbeatParam);

  createParam(TEST_DEVICE_INPUT_STRING, asynParamOctet, &_testDeviceInputParam);
  createParam(TEST_ANALOG_DEVICE_STRING, asynParamOctet, &_testAnalogDeviceParam);
  createParam(TEST_CHECK_FAULTS_STRING, asynParamInt32, &_testCheckFaultsParam);
  createParam(TEST_CHECK_BYPASS_STRING, asynParamInt32, &_testCheckBypassParam);

  setIntegerParam(0, _mpsStateParam, MPS_STATE_IDLE);

  // Initialize bypass date strings
  for (int i = 0; i < 100; ++i) {
    setStringParam(i, _mpsDeviceInputBypassExpirationDateStringParam, "Bypass date not set");
    setStringParam(i, _mpsAnalogDeviceBypassExpirationDateStringParam, "Bypass date not set");
  }

  // Start thread that sends out history messages
  History::getInstance().startSenderThread(historyServer, historyPort);

  //Need to get info from Firmware directly or through Engine...
  setStringParam(0, _mpsFwBuildStampParam, "XXXX");//reinterpret_cast<char *>(Firmware::getInstance().buildStamp));
  setStringParam(0, _mpsEngineStartTimeStringParam, "** Engine not started **");

  setStringParam(0, _mpsConfigDbMd5SumParam, "invalid");
  setStringParam(0, _mpsConfigDbUserParam, "invalid");
  setStringParam(0, _mpsConfigDbDateParam, "invalid");
  setStringParam(0, _mpsConfigDbSourceParam, "invalid");

  int value = 0;
  if (Firmware::getInstance().getEnable()) value = 1;
  setUIntDigitalParam(0, _mpsEnableRbvParam, value, 1);
  value = 0;
  if (Firmware::getInstance().getSoftwareEnable()) value = 1;
  setUIntDigitalParam(0, _mpsSwEnableRbvParam, value, 1);
  value = 0;
  if (Firmware::getInstance().getTimingCheckEnable()) value = 1;
  setUIntDigitalParam(0, _mpsTimingCheckEnableRbvParam, value, 1);
  value = 0;
  if (Firmware::getInstance().getEvaluationEnable()) value = 1;
  setUIntDigitalParam(0, _mpsEvaluationEnableRbvParam, value, 1);
  value = 0;
  if (Firmware::getInstance().getTimeoutEnable()) value = 1;
  setUIntDigitalParam(0, _mpsTimeoutEnableRbvParam, value, 1);
}

CentralNodeDriver::~CentralNodeDriver() {
  History::getInstance().stopSenderThread();
}

// asynStatus CentralNodeDriver::drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName,
// 					    size_t *psize) {
//   int addr;
//   getAddress(pasynUser, &addr);
//   LOG_TRACE("DRIVER", "drvUserCreate info=" << drvInfo << " timeout=" << pasynUser->timeout
// 	    << "; address=" << addr);
//   return asynSuccess;
// }
/*
asynStatus CentralNodeDriver::readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual) {
  if (_engineStartTimeStringParam == pasynUser->reason) {
    time_t startTime = Engine::getInstance().getStartTime();
    setStringParam(0, _engineStartTimeStringParam, ctime(&startTime));
  }
  *nActual = maxChars;
  return asynSuccess;
}
*/
asynStatus CentralNodeDriver::writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual) {
  asynStatus status = asynSuccess;

  // Set the parameter in the parameter library
  status = setStringParam(pasynUser->reason, (char *) value);

  if (_mpsConfigLoadParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Received request to load configuration: " << value);
    setIntegerParam(0, _mpsStateParam, MPS_STATE_LOAD_CONFIG);
    status = loadConfig(value);
    if (status != asynSuccess) {
      setIntegerParam(0, _mpsStateParam, MPS_STATE_LOAD_CONFIG_ERROR);
    }
    else {
      setIntegerParam(0, _mpsStateParam, MPS_STATE_RUNNING);
    }
  }
  else if (_testDeviceInputParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test device inputs from file: " << value);
    status = loadTestDeviceInputs(value);
  }
  else if (_testAnalogDeviceParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test analog devices from file: " << value);
    status = loadTestAnalogDevices(value);
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
  }
  *nActual = maxChars;

  return status;
}

asynStatus CentralNodeDriver::writeInt32(asynUser *pasynUser, epicsInt32 value) {
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized, reason="
	      << pasynUser->reason << ", addr=" << addr << ", value=" << value);
    return setIntegerParam(addr, pasynUser->reason, value);
  }

  LOG_TRACE("DRIVER", "INFO: reason=" << pasynUser->reason << ", addr=" << addr << ", value=" << value);
  if (_testCheckFaultsParam == pasynUser->reason) {
    Engine::getInstance().checkFaults();
  }
  else if (_mpsUnlatchAllParam == pasynUser -> reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      Engine::getInstance().getCurrentDb()->unlatchAll();
    }
  }
  else if (_mpsDeviceInputBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_DIGITAL, addr, 0, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsDeviceInputBypassExpirationDateParam, 0);
    }
  }
  else if (_mpsAnalogDeviceBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_ANALOG, addr, pasynUser->timeout, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsAnalogDeviceBypassExpirationDateParam, 0);
    }
  }
  else if (_testCheckBypassParam == pasynUser->reason) {
    Engine::getInstance().getBypassManager()->checkBypassQueue();
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readInt32(asynUser *pasynUser, epicsInt32 *value) {
  // TODO: check if 'addr' is valid
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);
  int bitIndex = pasynUser->timeout;

  if (_mpsFaultReasonParam == pasynUser->reason) {
    try {
      *value = Firmware::getInstance().getFaultReason();
    } catch (std::exception &e) {
      status = asynError;
    }
    return status;
  }
  else if (_mpsUpdateRateParam == pasynUser->reason) {
    *value = Engine::getInstance().getUpdateRate();
    return status;
  } 
  else if (_mpsUpdateCounterParam == pasynUser->reason) {
    *value = Engine::getInstance().getUpdateCounter();

    time_t startTime = Engine::getInstance().getStartTime();
    setStringParam(0, _mpsEngineStartTimeStringParam, ctime(&startTime));

    return status;
  } 
  else if (_mpsStateParam == pasynUser->reason) {
    getIntegerParam(_mpsStateParam, value);
    return status;
  }
  else if (_mpsFwSoftwareWdogCounterParam == pasynUser->reason) {
    *value = (epicsInt32)(Engine::getInstance().getWdErrorCnt());
    return status;
  }
  else if (_mpsFwMonitorNotReadyCounterParam == pasynUser->reason) {
    //*value = Firmware::getInstance()._monitorNotReadyCounter;
    // TO BE DONE: We should rething if this is neccesaty and how to implement it, maybe in FW instead?
    *value = 0;
    return status;
  }

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    //    LOG_TRACE("DRIVER", "ERROR: Database not initialized");
    //    return asynError;
    return status;
  }

  if (_mpsSwMitigationParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->beamDestinations->find(addr) ==
	    Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
	  LOG_TRACE("DRIVER", "ERROR: BeamDestination not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->allowedBeamClass) {
	  *value = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->allowedBeamClass->number;
	}
	else {
	  LOG_TRACE("DRIVER", "ERROR: Invalid allowed class for mitigation device "
		    << Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->name);
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceBypassStatusParam ==  pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->analogDevices->find(addr) ==
	    Engine::getInstance().getCurrentDb()->analogDevices->end()) {
	  LOG_TRACE("DRIVER", "ERROR: AnalogDevice not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass[bitIndex]->status == BYPASS_VALID) {
	  *value = 1;
	}
	else {
	  *value = 0;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsFwMitigationParam == pasynUser->reason) {
    try {
      uint8_t index = 0;
      uint8_t bitShift = 0;
      {
	std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
	if (Engine::getInstance().getCurrentDb()->beamDestinations->find(addr) ==
	    Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
	  LOG_TRACE("DRIVER", "ERROR: BeamDestination not found, key=" << addr);
	  return asynError;
	}
	index = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->softwareMitigationBufferIndex;
	bitShift = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->bitShift;
      }

      uint32_t fwMitigation[2];
      
      Firmware::getInstance().getFirmwareMitigation(&fwMitigation[0]);
      *value = (fwMitigation[index] >> bitShift) & 0xF;
    } catch (std::exception &e) {
      status = asynError;
    }
  }
  else if (_mpsMitigationParam == pasynUser->reason) {
    try {
      uint8_t index = 0;
      uint8_t bitShift = 0;
      {
	std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
	if (Engine::getInstance().getCurrentDb()->beamDestinations->find(addr) ==
	    Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
	  LOG_TRACE("DRIVER", "ERROR: BeamDestination not found, key=" << addr);
	  return asynError;
	}
	index = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->softwareMitigationBufferIndex;
	bitShift = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->bitShift;
      }
      
      uint32_t mitigation[2];
      Firmware::getInstance().getMitigation(&mitigation[0]);
      *value = (mitigation[index] >> bitShift) & 0xF;
    } catch (std::exception &e) {
      status = asynError;
    }
  }
  else if (_mpsLatchedMitigationParam == pasynUser->reason) {
    try {	  
      uint8_t index = 0;
      uint8_t bitShift = 0;
      {
	std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
	if (Engine::getInstance().getCurrentDb()->beamDestinations->find(addr) ==
	    Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
	  LOG_TRACE("DRIVER", "ERROR: BeamDestination not found, key=" << addr);

	  return asynError;
	}
	index = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->softwareMitigationBufferIndex;
	bitShift = Engine::getInstance().getCurrentDb()->beamDestinations->at(addr)->bitShift;
      }
      
      uint32_t mitigation[2];
      Firmware::getInstance().getLatchedMitigation(&mitigation[0]);
      *value = (mitigation[index] >> bitShift) & 0xF;
    } catch (std::exception &e) {
      status = asynError;
    }
  }
  else if (_mpsEvalCycleMaxParam == pasynUser->reason) {
    *value = Engine::getInstance().getMaxEvalTime();
    return status;
  }
  else if (_mpsEvalCycleAvgParam == pasynUser->reason) {
    *value = Engine::getInstance().getAvgEvalTime();
    return status;
  }
  else if (_mpsEvalMaxParam == pasynUser->reason) {
    *value = Engine::getInstance().getMaxCheckTime();
    return status;
  }
  else if (_mpsEvalAvgParam == pasynUser->reason) {
    *value = Engine::getInstance().getAvgCheckTime();
    return status;
  }
  else if (_mpsUpdateMaxParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->getMaxUpdateTime();
    return status;
  }
  else if (_mpsUpdateAvgParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->getAvgUpdateTime();
    return status;
  }
  else if (_mpsFwUpdateMaxParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->getMaxFwUpdatePeriod();
    return status;
  }
  else if (_mpsWdUpdateMaxParam == pasynUser->reason) {
    *value = Engine::getInstance().getMaxWdUpdatePeriod();
    return status;
  }
  else if (_mpsFwUpdateAvgParam == pasynUser->reason) {
    *value = Engine::getInstance().getCurrentDb()->getAvgFwUpdatePeriod();
    return status;
  }
  else if (_mpsWdUpdateAvgParam == pasynUser->reason) {
    *value = Engine::getInstance().getAvgWdUpdatePeriod();
    return status;
  }
  else if (_mpsAppTimestampLowBitsParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      uint64_t time = Engine::getInstance().getCurrentDb()->getFastUpdateTimeStamp();
      *value = time & 0xFFFFFFFF;
    }
    return status;
  }
  else if (_mpsAppTimestampHighBitsParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      uint64_t time = Engine::getInstance().getCurrentDb()->getFastUpdateTimeStamp();
      *value = (time >> 32) & 0xFFFFFFFF;
    }
    return status;
  }
  else if (_mpsDeviceInputBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
	if (Engine::getInstance().getCurrentDb()->deviceInputs->find(addr) ==
	    Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
	  LOG_TRACE("DRIVER", "ERROR: DeviceInput not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->status == BYPASS_VALID) {
	  time_t now = time(0);
	  *value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->until - now;
	}
	else {
	  *value = 0;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
	if (Engine::getInstance().getCurrentDb()->analogDevices->find(addr) ==
	    Engine::getInstance().getCurrentDb()->analogDevices->end()) {
	  LOG_TRACE("DRIVER", "ERROR: AnalogDevice not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass[bitIndex]->status == BYPASS_VALID) {
	  time_t now = time(0);
	  *value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass[bitIndex]->until - now;
	}
	else {
	  *value = 0;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request (reason " << pasynUser->reason << ")");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask) {
  // TODO: check if 'addr' is valid
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);

  if (_mpsEnableRbvParam == pasynUser->reason) {
    if (Firmware::getInstance().getEnable()) *value = 1; else *value = 0;
    return status;
  }
  else if (_mpsSwEnableRbvParam == pasynUser->reason) {
    if (Firmware::getInstance().getSoftwareEnable()) *value = 1; else *value = 0;
    return status;
  }
  else if (_mpsTimingCheckEnableRbvParam == pasynUser->reason) {
    if (Firmware::getInstance().getTimingCheckEnable()) *value = 1; else *value = 0;
    return status;
  }
  else if (_mpsEvaluationEnableRbvParam == pasynUser->reason) {
    if (Firmware::getInstance().getEvaluationEnable()) *value = 1; else *value = 0;
    return status;
  }
  else if (_mpsTimeoutEnableRbvParam == pasynUser->reason) {
    if (Firmware::getInstance().getTimeoutEnable()) *value = 1; else *value = 0;
    return status;
  }


  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    //    LOG_TRACE("DRIVER", "ERROR: Database not initialized");
    return status;
  }

  if (_mpsDeviceInputParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->deviceInputs->find(addr) ==
	    Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
	  LOG_TRACE("DRIVER", "ERROR: DeviceInput not found, key=" << addr);
	  return asynError;
	}
	*value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->value;
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->analogDevices->find(addr) ==
	    Engine::getInstance().getCurrentDb()->analogDevices->end()) {
	  LOG_TRACE("DRIVER", "ERROR: AnalogDevice not found, key=" << addr);
	  return asynError;
	}
	// Non-zero *value means threshold exceeded
	*value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->value & mask;
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsFaultParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
	    Engine::getInstance().getCurrentDb()->faults->end()) {
	  LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->faults->at(addr)->faulted) {
	  *value = 1;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsFaultIgnoredParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
	    Engine::getInstance().getCurrentDb()->faults->end()) {
	  LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->faults->at(addr)->ignored) {
	  *value = 1;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceIgnoredParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->analogDevices->find(addr) ==
	    Engine::getInstance().getCurrentDb()->analogDevices->end()) {
	  LOG_TRACE("DRIVER", "ERROR: AnalogDevice not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->ignored) {
	  *value = 1;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsDeviceInputLatchedParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->deviceInputs->find(addr) ==
	    Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
	  LOG_TRACE("DRIVER", "ERROR: DeviceInput not found, key=" << addr);
	  return asynError;
	}
	*value = Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->latchedValue;
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsFaultLatchedParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->faults->at(addr)->faultLatched) {
	  *value = 1;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsDeviceInputBypassStatusParam ==  pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->deviceInputs->find(addr) ==
	    Engine::getInstance().getCurrentDb()->deviceInputs->end()) {
	  LOG_TRACE("DRIVER", "ERROR: DeviceInput not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->status == BYPASS_VALID) {
	  *value = 1;
	}
	else {
	  *value = 0;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceLatchedParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->analogDevices->find(addr) ==
	    Engine::getInstance().getCurrentDb()->analogDevices->end()) {
	  LOG_TRACE("DRIVER", "ERROR: AnalogDevice not found, key=" << addr);
	  return asynError;
	}
	// mask
	// Non-zero *value means threshold exceeded
	*value = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->latchedValue & mask;
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAppStatusParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->applicationCards->find(addr) ==
	    Engine::getInstance().getCurrentDb()->applicationCards->end()) {
	  LOG_TRACE("DRIVER", "ERROR: ApplicationCard not found, key=" << addr);
	  std::cout << "*** ERROR: ApplicationCard not found, key=" << addr << std::endl;
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->online) {
	  *value = 1;
	}
	else {
	  *value = 0;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsConditionParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	if (Engine::getInstance().getCurrentDb()->conditions->find(addr) ==
	    Engine::getInstance().getCurrentDb()->conditions->end()) {
	  LOG_TRACE("DRIVER", "ERROR: Condition not found, key=" << addr);
	  return asynError;
	}
	if (Engine::getInstance().getCurrentDb()->conditions->at(addr)->state) {
	  *value = 1;
	}
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask) {
  asynStatus status = asynSuccess;
  int addr;
  getAddress(pasynUser, &addr);
  int bitIndex = pasynUser->timeout;

  if (_mpsEnableParam == pasynUser->reason) {
    bool enable = false;
    if (value != 0) {
      enable = true;
    }
    Firmware::getInstance().setEnable(enable);
    return status;
  }
  else if (_mpsSwEnableParam == pasynUser->reason) {
    bool enable = false;
    if (value != 0) {
      enable = true;
    }
    Firmware::getInstance().setSoftwareEnable(enable);
    return status;
  }
  else if (_mpsEvaluationEnableParam == pasynUser->reason) {
    bool enable = false;
    if (value != 0) {
      enable = true;
    }
    Firmware::getInstance().setEvaluationEnable(enable);
    return status;
  }
  else if (_mpsTimingCheckEnableParam == pasynUser->reason) {
    bool enable = false;
    if (value != 0) {
      enable = true;
    }
    Firmware::getInstance().setTimingCheckEnable(enable);
    return status;
  }
  else if (_mpsTimeoutEnableParam == pasynUser->reason) {
    bool enable = false;
    if (value != 0) {
      enable = true;
    }
    Firmware::getInstance().setTimeoutEnable(enable);
    return status;
  }
  else if (_mpsMitigationUnlatchParam == pasynUser->reason) {
    Firmware::getInstance().evalLatchClear();
    return status;
  }
  else if (_mpsMonErrClearParam == pasynUser->reason) {
    Firmware::getInstance().monErrClear();
    return status;
  }
  else if (_mpsSwErrClearParam == pasynUser->reason) {
    Firmware::getInstance().swErrClear();
    return status;
  }
  else if (_mpsToErrClearParam == pasynUser->reason) {
    Firmware::getInstance().toErrClear();
    return status;
  }
  else if (_mpsMoConcErrClearParam == pasynUser->reason) {
    Firmware::getInstance().moConcErrClear();
    return status;
  }
  else if (_mpsSkipHeartbeatParam == pasynUser->reason) {
    //Firmware::getInstance()._skipHeartbeat = value;
    // TO BE DONE: Is this neccesary? If so, we need to reimplement it.
    return status;
  }

  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized (reason=" << pasynUser->reason
	      << ", addr=" << addr << ")");
    status = setUIntDigitalParam(addr, pasynUser->reason, value, mask);
    return status;
  }

  if (_mpsDeviceInputUnlatchParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->unlatch();
	status = setUIntDigitalParam(addr, pasynUser->reason,
				     Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->latchedValue, mask);
	LOG_TRACE("DRIVER", "Unlatch: "
		  << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name
		  << " value: " << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->latchedValue);
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceUnlatchParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	uint32_t latchedValue = Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->unlatch(mask);
	
	status = setUIntDigitalParam(addr, pasynUser->reason, latchedValue, mask);
	LOG_TRACE("DRIVER", "Unlatch: "
		  << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->channel->name
		  << " latchedValue: " << latchedValue << ", unlatched: "
		  << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->latchedValue 
		  << " mask: " << mask);
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsFaultUnlatchParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	Engine::getInstance().getCurrentDb()->faults->at(addr)->unlatch();
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsDeviceInputBypassValueParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->bypass->value = value;
	LOG_TRACE("DRIVER", "BypassValue: "
		  << Engine::getInstance().getCurrentDb()->deviceInputs->at(addr)->channel->name
		  << " value: " << value);
      } catch (std::exception &e) {
	status = asynError;
      }
    }
  }
  else if (_mpsAnalogDeviceBypassValueParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
	Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->bypass[bitIndex]->value = value;
	LOG_TRACE("DRIVER", "BypassValue: "
		  << Engine::getInstance().getCurrentDb()->analogDevices->at(addr)->channel->name
		  << " value: " << value << " (threshold=" << bitIndex << ")");
      } catch (const std::out_of_range &e) {
	LOG_TRACE("DRIVER", "ERROR: AnalogDevices out of range, key=" << addr);
      }
    }
  }
  else if (_mpsUpdateClearParam == pasynUser->reason) {
    Engine::getInstance().getCurrentDb()->clearUpdateTime();
    return status;
  }
  else {
    LOG_TRACE("DRIVER", "Unknown parameter, ignoring request");
    status = asynError;
  }
  return status;
}

asynStatus CentralNodeDriver::loadConfig(const char *config) {
  std::string fullName = _configPath + "/" + config;
  time_t now;
  time(&now);

  std::cout << "[" << ctime(&now) << "] INFO: Loading config " << fullName << std::endl;
  try {
    if (Engine::getInstance().loadConfig(fullName, _inputUpdateTimeout) != 0) {
      std::cerr << "ERROR: Failed to load configuration " << fullName << std::endl; 
      return asynError;
    }
  } catch (DbException ex) {
    std::cerr << "ERROR: Exception loading config " << fullName << std::endl;
    std::cerr << ex.what() << std::endl;
    return asynError;
  } catch (CentralNodeException ex) {
    std::cerr << "ERROR: Exception loading config " << fullName << std::endl;
    std::cerr << ex.what() << std::endl;
    return asynError;
  }
  LOG_TRACE("DRIVER", "Loaded configuration: " << fullName);

  if (Engine::getInstance().getCurrentDb()->databaseInfo) {
    try {
      setStringParam(0, _mpsConfigDbMd5SumParam, Engine::getInstance().getCurrentDb()->databaseInfo->at(0)->md5sum.c_str());
      setStringParam(0, _mpsConfigDbUserParam, Engine::getInstance().getCurrentDb()->databaseInfo->at(0)->user.c_str());
      setStringParam(0, _mpsConfigDbDateParam, Engine::getInstance().getCurrentDb()->databaseInfo->at(0)->date.c_str());
      setStringParam(0, _mpsConfigDbSourceParam, Engine::getInstance().getCurrentDb()->databaseInfo->at(0)->source.c_str());
    } catch (std::exception ex) {
      std::cerr << "ERROR: Invalid database info" << std::endl;
      return asynError;
    }
  }

  return asynSuccess;
}

asynStatus CentralNodeDriver::loadTestDeviceInputs(const char *testFilename) {
  asynStatus status = asynSuccess;
  std::string fullName = _configPath + "../inputs/" + testFilename;

  std::ifstream testInputFile;
  testInputFile.open(fullName.c_str(), std::ifstream::in);
  if (!testInputFile.is_open()) {
    std::cerr << "ERROR: Failed to load test input file " << fullName << std::endl; 
    return asynError;
  }

  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    while (!testInputFile.eof()) {
      int deviceId;
      int deviceValue;
      testInputFile >> deviceId;
      testInputFile >> deviceValue;
      
      try {
	Engine::getInstance().getCurrentDb()->deviceInputs->at(deviceId)->update(deviceValue);
      } catch (std::exception ex) {
	std::cerr << "ERROR: Invalid device input ID: " << deviceId << std::endl;
	status = asynError;
      }
    } 
  }

  return status;
}

asynStatus CentralNodeDriver::loadTestAnalogDevices(const char *testFilename) {
  asynStatus status = asynSuccess;
  std::string fullName = _configPath + "../inputs/" + testFilename;

  std::ifstream testInputFile;
  testInputFile.open(fullName.c_str(), std::ifstream::in);
  if (!testInputFile.is_open()) {
    std::cerr << "ERROR: Failed to load test input file " << fullName << std::endl; 
    return asynError;
  }

  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    while (!testInputFile.eof()) {
      int deviceId;
      int deviceValue;
      testInputFile >> deviceId;
      testInputFile >> deviceValue;
      
      try {
	Engine::getInstance().getCurrentDb()->analogDevices->at(deviceId)->update(deviceValue);
      } catch (std::exception ex) {
	std::cerr << "ERROR: Invalid analog device ID: " << deviceId << std::endl;
	status = asynError;
      }
    } 
  }

  return status;
}

/**
 * @param expirationTime bypass expiration time in seconds since now.
 * If expirationTime is zero or negative the bypass is cancelled.
 */
asynStatus CentralNodeDriver::setBypass(BypassType bypassType, int deviceId,
					int thresholdIndex, epicsInt32 expirationTime) {
  asynStatus status = asynSuccess;
  time_t now;
  time(&now);

  LOG_TRACE("DRIVER", "Set bypass for device " << deviceId << ", thresholdIndex=" << thresholdIndex);

  /*
  if (expirationTime > 0 && expirationTime < now) {
    std::cerr << "WARNING: Invalid expiration time " << expirationTime
	      << ". Must be greater than (now) " << now << "." << std::endl;
    return asynError;
  }
  */

  // Add expirationTime to current time, unless the bypass is being cancelled
  if (expirationTime > 0) {
    expirationTime += now;
  }
  else {
    expirationTime = 0;
  }

  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    try {
      if (bypassType == BYPASS_DIGITAL) {
	uint32_t bypassValue = Engine::getInstance().getCurrentDb()->deviceInputs->at(deviceId)->bypass->value;
	Engine::getInstance().getBypassManager()->setBypass(Engine::getInstance().getCurrentDb(), bypassType,
							    deviceId, bypassValue, expirationTime);
      }
      else {
	uint32_t bypassValue = Engine::getInstance().getCurrentDb()->analogDevices->at(deviceId)->bypass[thresholdIndex]->value;
	Engine::getInstance().getBypassManager()->setThresholdBypass(Engine::getInstance().getCurrentDb(), bypassType,
								     deviceId, bypassValue, expirationTime, thresholdIndex);
      }
    } catch (std::exception &e) {
      return asynError;
    }
  }

  if (expirationTime == 0) {
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(deviceId, _mpsDeviceInputBypassExpirationDateStringParam, "Not Bypassed");
    }
    else {
      status = setStringParam(deviceId, _mpsAnalogDeviceBypassExpirationDateStringParam, "Not Bypassed");
    }
  }
  else {
    time_t expTime = expirationTime;
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(deviceId, _mpsDeviceInputBypassExpirationDateStringParam, ctime(&expTime));
    }
    else {
      status = setStringParam(deviceId, _mpsAnalogDeviceBypassExpirationDateStringParam, ctime(&expTime));
    }
  }

  return status;
}

void CentralNodeDriver::printQueue() {
  Engine::getInstance().getBypassManager()->printBypassQueue();
}

void CentralNodeDriver::showMitigation() {
  Engine::getInstance().showBeamDestinations();
}

void CentralNodeDriver::showFaults() {
  Engine::getInstance().showFaults();
}

void CentralNodeDriver::showFirmware() {
  Engine::getInstance().showFirmware();
}

void CentralNodeDriver::showDatabaseInfo() {
  Engine::getInstance().showDatabaseInfo();
}

void CentralNodeDriver::showEngineInfo() {
  Engine::getInstance().showStats();
}

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

