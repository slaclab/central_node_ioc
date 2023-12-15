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
  asynPortDriver(portName, 10000,
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
  createParam(MPS_DIGITAL_CHANNEL_STRING, asynParamUInt32Digital, &_mpsDigitalChannelParam);
  createParam(MPS_ANALOG_CHANNEL_STRING, asynParamUInt32Digital, &_mpsAnalogChannelParam);
  createParam(MPS_SW_MITIGATION_STRING, asynParamInt32, &_mpsSwMitigationParam);
  createParam(MPS_FAULT_STRING, asynParamUInt32Digital, &_mpsFaultParam);
  createParam(MPS_FAULT_TEST_STRING, asynParamInt32, &_mpsFaultParamTest);
  createParam(MPS_FAULT_FAULTED_STRING, asynParamUInt32Digital, &_mpsFaultFaultedParam);
  createParam(MPS_FAULT_ACTIVE_STRING, asynParamUInt32Digital, &_mpsFaultActiveParam);
  createParam(MPS_FAULT_IGNORED_STRING, asynParamUInt32Digital, &_mpsFaultIgnoredParam);
  createParam(MPS_FAULT_BYPV_STRING, asynParamInt32, &_mpsFaultBypassValueParam);
  createParam(MPS_FAULT_BYPS_STRING, asynParamUInt32Digital, &_mpsFaultBypassStatusParam);
  createParam(MPS_FAULT_BYPEXPDATE_STRING, asynParamInt32, &_mpsFaultBypassExpirationDateParam);
  createParam(MPS_FAULT_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsFaultBypassRemainingExpirationTimeParam);
  createParam(MPS_FAULT_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsFaultBypassExpirationDateStringParam);
  createParam(MPS_DIGITAL_CHANNEL_LATCHED_STRING, asynParamUInt32Digital, &_mpsDigitalChannelLatchedParam);
  createParam(MPS_DIGITAL_CHANNEL_UNLATCH_STRING, asynParamUInt32Digital, &_mpsDigitalChannelUnlatchParam);
  createParam(MPS_DIGITAL_CHANNEL_UNLATCH_FAST_STRING, asynParamUInt32Digital, &_mpsDigitalChannelUnlatchFastParam);
  createParam(MPS_DIGITAL_CHANNEL_BYPV_STRING, asynParamUInt32Digital, &_mpsDigitalChannelBypassValueParam);
  createParam(MPS_DIGITAL_CHANNEL_BYPS_STRING, asynParamUInt32Digital, &_mpsDigitalChannelBypassStatusParam);
  createParam(MPS_DIGITAL_CHANNEL_BYPEXPDATE_STRING, asynParamInt32, &_mpsDigitalChannelBypassExpirationDateParam);
  createParam(MPS_DIGITAL_CHANNEL_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsDigitalChannelBypassRemainingExpirationTimeParam);
  createParam(MPS_DIGITAL_CHANNEL_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsDigitalChannelBypassExpirationDateStringParam);
  createParam(MPS_ANALOG_CHANNEL_LATCHED_STRING, asynParamUInt32Digital, &_mpsAnalogChannelLatchedParam);
  createParam(MPS_ANALOG_CHANNEL_UNLATCH_STRING, asynParamUInt32Digital, &_mpsAnalogChannelUnlatchParam);
  createParam(MPS_ANALOG_CHANNEL_BYPV_STRING, asynParamUInt32Digital, &_mpsAnalogChannelBypassValueParam);
  createParam(MPS_ANALOG_CHANNEL_BYPS_STRING, asynParamInt32, &_mpsAnalogChannelBypassStatusParam);
  createParam(MPS_ANALOG_CHANNEL_BYPEXPDATE_STRING, asynParamInt32, &_mpsAnalogChannelBypassExpirationDateParam);
  createParam(MPS_ANALOG_CHANNEL_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsAnalogChannelBypassRemainingExpirationTimeParam);
  createParam(MPS_ANALOG_CHANNEL_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsAnalogChannelBypassExpirationDateStringParam);
  createParam(MPS_ANALOG_CHANNEL_IGNORED_STRING, asynParamUInt32Digital, &_mpsAnalogChannelIgnoredParam);
  createParam(MPS_ANALOG_CHANNEL_IGNORED_INTEGRATOR_STRING, asynParamUInt32Digital, &_mpsAnalogChannelIgnoredIntegratorParam);
  createParam(MPS_UNLATCH_ALL_STRING, asynParamInt32, &_mpsUnlatchAllParam);
  createParam(MPS_FW_BUILD_STAMP_STRING_STRING, asynParamOctet, &_mpsFwBuildStampParam);
  createParam(MPS_SW_UPDATE_RATE_STRING, asynParamInt32, &_mpsUpdateRateParam);
  createParam(MPS_FW_MITIGATION_STRING, asynParamInt32, &_mpsFwMitigationParam);
  createParam(MPS_MITIGATION_STRING, asynParamInt32, &_mpsMitigationParam);
  createParam(MPS_SW_UPDATE_COUNTER_STRING, asynParamInt32, &_mpsUpdateCounterParam);
  createParam(MPS_ENGINE_START_TIME_STRING_STRING, asynParamOctet, &_mpsEngineStartTimeStringParam);
  createParam(MPS_LATCHED_MITIGATION_STRING, asynParamInt32, &_mpsLatchedMitigationParam);
  createParam(MPS_FINAL_MITIGATION_STRING, asynParamInt32, &_mpsFinalBeamClassParam);
  createParam(MPS_MITIGATION_UNLATCH_STRING, asynParamUInt32Digital, &_mpsMitigationUnlatchParam);
  createParam(MPS_APP_BYPS_STRING, asynParamUInt32Digital, &_mpsAppBypassStatusParam);
  createParam(MPS_APP_BYPEXPDATE_STRING, asynParamInt32, &_mpsAppBypassExpirationDateParam);
  createParam(MPS_APP_REMAINING_BYPTIME_STRING, asynParamInt32, &_mpsAppBypassRemainingExpirationTimeParam);
  createParam(MPS_APP_BYPEXPDATE_STRING_STRING, asynParamOctet, &_mpsAppBypassExpirationDateStringParam);
  createParam(MPS_APP_STATUS_STRING, asynParamUInt32Digital, &_mpsAppStatusParam);
  createParam(MPS_APP_TIMESTAMP_LOW_BITS_STRING, asynParamInt32, &_mpsAppTimestampLowBitsParam);
  createParam(MPS_APP_TIMESTAMP_HIGH_BITS_STRING, asynParamInt32, &_mpsAppTimestampHighBitsParam);
  createParam(MPS_MON_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsMonErrClearParam);
  createParam(MPS_SW_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsSwErrClearParam);
  createParam(MPS_TO_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsToErrClearParam);
  createParam(MPS_MO_CONC_ERR_CLEAR_STRING, asynParamUInt32Digital, &_mpsMoConcErrClearParam);
  createParam(MPS_BEAM_FAULT_CLEAR_STRING, asynParamUInt32Digital, &_mpsBeamFaultClearParam);
  createParam(MPS_EVALCYCLE_AVG_TIME_STRING, asynParamInt32, &_mpsEvalCycleAvgParam);
  createParam(MPS_EVALCYCLE_MAX_TIME_STRING, asynParamInt32, &_mpsEvalCycleMaxParam);
  createParam(MPS_EVAL_AVG_TIME_STRING, asynParamInt32, &_mpsEvalAvgParam);
  createParam(MPS_EVAL_MAX_TIME_STRING, asynParamInt32, &_mpsEvalMaxParam);
  createParam(MPS_UPDATE_AVG_TIME_STRING, asynParamInt32, &_mpsUpdateAvgParam);
  createParam(MPS_UPDATE_MAX_TIME_STRING, asynParamInt32, &_mpsUpdateMaxParam);
  createParam(MPS_UPDATE_TIME_CLEAR_STRING, asynParamUInt32Digital, &_mpsUpdateClearParam);
  createParam(MPS_FWUPDATE_AVG_PERIOD_STRING, asynParamInt32, &_mpsFwUpdateAvgParam);
  createParam(MPS_FWUPDATE_MAX_PERIOD_STRING, asynParamInt32, &_mpsFwUpdateMaxParam);
  createParam(MPS_WDUPDATE_AVG_PERIOD_STRING, asynParamInt32, &_mpsWdUpdateAvgParam);
  createParam(MPS_WDUPDATE_MAX_PERIOD_STRING, asynParamInt32, &_mpsWdUpdateMaxParam);
  createParam(MPS_CONFIG_DB_SRC_STRING, asynParamOctet, &_mpsConfigDbSourceParam);
  createParam(MPS_CONFIG_DB_USER_STRING, asynParamOctet, &_mpsConfigDbUserParam);
  createParam(MPS_CONFIG_DB_DATE_STRING, asynParamOctet, &_mpsConfigDbDateParam);
  createParam(MPS_CONFIG_DB_MD5SUM_STRING, asynParamOctet, &_mpsConfigDbMd5SumParam);
  createParam(MPS_STATE_STRING, asynParamInt32, &_mpsStateParam);
  createParam(MPS_IGNORE_CONDITION_STRING, asynParamUInt32Digital, &_mpsIgnoreConditionParam);
  createParam(MPS_FW_SOFTWARE_WDOG_COUNTER_STRING, asynParamInt32, &_mpsFwSoftwareWdogCounterParam);
  createParam(MPS_FORCE_DEST_STRING, asynParamUInt32Digital, &_mpsForceDestBeamClass);
  createParam(MPS_SW_PERMIT_DEST_STRING, asynParamUInt32Digital, &_mpsSoftPermitDestBeamClass);
  createParam(MPS_FW_RESET_ALL_STRING, asynParamUInt32Digital, &_mpsFwResetAll);
  createParam(MPS_TIMING_BC_STRING, asynParamInt32, &_mpsTimingBCparam);
  createParam(MPS_MAX_BEAM_CLASS_STRING, asynParamUInt32Digital, &_mpsMaxPermitDestBeamClass);
  createParam(MPS_TEST_MODE_STRING, asynParamUInt32Digital, &_mpsTestMode);


  createParam(TEST_DIGITAL_CHANNEL_STRING, asynParamOctet, &_testDigitalChannelParam);
  createParam(TEST_ANALOG_CHANNEL_STRING, asynParamOctet, &_testAnalogChannelParam);
  createParam(TEST_CHECK_FAULTS_STRING, asynParamInt32, &_testCheckFaultsParam);
  createParam(TEST_CHECK_BYPASS_STRING, asynParamInt32, &_testCheckBypassParam);

  createParam(MPS_APP_TIMEOUT_ENABLE_STRING, asynParamUInt32Digital, &_mpsAppTimeoutEnableParam);
  createParam(MPS_APP_ACTIVE_ENABLE_STRING, asynParamUInt32Digital, &_mpsAppActiveEnableParam);

  setIntegerParam(0, _mpsStateParam, MPS_STATE_IDLE);

  // Initialize bypass date strings
  for (int i = 0; i < 10000; ++i) {
    setStringParam(i, _mpsDigitalChannelBypassExpirationDateStringParam, "Bypass date not set");
    setStringParam(i, _mpsAnalogChannelBypassExpirationDateStringParam, "Bypass date not set");
    setStringParam(i, _mpsFaultBypassExpirationDateStringParam, "Bypass date not set");
    setStringParam(i, _mpsAppBypassExpirationDateStringParam, "Bypass date not set");
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

  _testMode = 1;
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
  else if (_testDigitalChannelParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test fault inputs from file: " << value);
    status = loadTestDigitalChannels(value);
  }
  else if (_testAnalogChannelParam == pasynUser->reason) {
    LOG_TRACE("DRIVER", "Loading test analog channels from file: " << value);
    status = loadTestAnalogChannels(value);
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
    if ( Engine::getInstance().unlatchAllowed() ){
      {
        std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
        Engine::getInstance().getCurrentDb()->unlatchAll();
      }
      Engine::getInstance().clearSoftwareLatch();
      Firmware::getInstance().evalLatchClear();
    }
    Engine::getInstance().startLatchTimeout();
  }
  else if (_mpsDigitalChannelBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_DIGITAL, addr, 0, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsDigitalChannelBypassExpirationDateParam, 0);
    }
  }
  else if (_mpsFaultBypassValueParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        Engine::getInstance().getCurrentDb()->faults->at(addr)->bypass->value = value;
        LOG_TRACE("DRIVER", "BypassValue: "
            << Engine::getInstance().getCurrentDb()->faults->at(addr)->name
            << " value: " << value);
      } catch (std::exception &e) {
	      status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_ANALOG, addr, pasynUser->timeout, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsAnalogChannelBypassExpirationDateParam, 0);
    }
  }
  else if (_mpsFaultBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_FAULT, addr, 0, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsFaultBypassExpirationDateParam, 0);
    }
  }
  else if (_mpsAppBypassExpirationDateParam == pasynUser->reason) {
    status = setBypass(BYPASS_APPLICATION, addr, 0, value);
    if (status != asynSuccess) {
      status = setIntegerParam(addr, _mpsAppBypassExpirationDateParam, 0);
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

  if (_mpsUpdateRateParam == pasynUser->reason) {
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
  else if (_mpsFaultParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
      	if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
      	    Engine::getInstance().getCurrentDb()->faults->end()) {
     	        LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
      	  return asynError;
      	}
        *value = Engine::getInstance().getCurrentDb()->faults->at(addr)->displayState;
      } catch (std::exception &e) {
          
        	status = asynError;
      }
    }
  }
  else if (_mpsFaultParamTest == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
      	if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
      	    Engine::getInstance().getCurrentDb()->faults->end()) {
     	        LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
      	  return asynError;
      	}
        *value = Engine::getInstance().getCurrentDb()->faults->at(addr)->value;
      } catch (std::exception &e) {
        	status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelBypassStatusParam ==  pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->bypass[bitIndex]->status == BYPASS_VALID) {
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
  else if (_mpsFinalBeamClassParam == pasynUser->reason) {
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
      Firmware::getInstance().getFinalBcL(&mitigation[1]);
      Firmware::getInstance().getFinalBcH(&mitigation[0]);
      *value = (mitigation[index] >> bitShift) & 0xF;
    } catch (std::exception &e) {
      status = asynError;
    }
  }
  else if (_mpsTimingBCparam == pasynUser->reason) {
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
      Firmware::getInstance().getTimingBcL(&mitigation[1]);
      Firmware::getInstance().getTimingBcH(&mitigation[0]);
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
  else if (_mpsDigitalChannelBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
        if (Engine::getInstance().getCurrentDb()->digitalChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->bypass->status == BYPASS_VALID) {
          time_t now = time(0);
          *value = Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->bypass->until - now;
        }
        else {
          *value = 0;
          status = setStringParam(addr, _mpsDigitalChannelBypassExpirationDateStringParam, "Not Bypassed");
        }
      } catch (std::exception &e) {
      	status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->bypass[bitIndex]->status == BYPASS_VALID) {
          time_t now = time(0);
          *value = Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->bypass[bitIndex]->until - now;
        }
        else {
          *value = 0;
          status = setStringParam(getParamStringOffset(addr, bitIndex),
                _mpsAnalogChannelBypassExpirationDateStringParam, "Not Bypassed");
        }
      } catch (std::exception &e) {
          status = asynError;
      }
    }
  }
  else if (_mpsFaultBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
        if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
            Engine::getInstance().getCurrentDb()->faults->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->faults->at(addr)->bypass->status == BYPASS_VALID) {
          time_t now = time(0);
          *value = Engine::getInstance().getCurrentDb()->faults->at(addr)->bypass->until - now;
        }
        else {
          *value = 0;
          status = setStringParam(addr, _mpsFaultBypassExpirationDateStringParam, "Not Bypassed");
        }
      } catch (std::exception &e) {
      	status = asynError;
      }
    }
  }
  else if (_mpsAppBypassRemainingExpirationTimeParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());

      try {
        if (Engine::getInstance().getCurrentDb()->applicationCards->find(addr) ==
            Engine::getInstance().getCurrentDb()->applicationCards->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->bypass->status == BYPASS_VALID) {
          time_t now = time(0);
          *value = Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->bypass->until - now;
        }
        else {
          *value = 0;
          status = setStringParam(addr, _mpsAppBypassExpirationDateStringParam, "Not Bypassed");
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

  if (_mpsAppTimeoutEnableParam == pasynUser->reason) {
    if (addr >= FW_NUM_APPLICATION_MASKS)
      return asynError;
    *value = Firmware::getInstance().getAppTimeoutEnable(addr);
    return status;
  }
  else if (_mpsTestMode == pasynUser->reason) {
    if (getTestMode()) {
      *value = 1;
    }
    else {
      *value = 0;
    }
    return status;
  }
  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    //    LOG_TRACE("DRIVER", "ERROR: Database not initialized");
    return status;
  }

  if (_mpsDigitalChannelParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->digitalChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        *value = Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->value;
      } catch (std::exception &e) {
	      status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannels not found, key=" << addr);
          return asynError;
        }
        // Non-zero *value means threshold exceeded
        *value = Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->value & mask;
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
  else if (_mpsFaultFaultedParam == pasynUser->reason) {
    {
      *value = 0;
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
  else if (_mpsFaultActiveParam == pasynUser->reason) {
    {
      *value = 0;
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
      	if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
      	    Engine::getInstance().getCurrentDb()->faults->end()) {
     	        LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
      	  return asynError;
      	}
	      if (Engine::getInstance().getCurrentDb()->faults->at(addr)->faultActive) {
          *value = 1;
        }
      } catch (std::exception &e) {
        	status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelIgnoredParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->ignored) {
          *value = 1;
        }
      } catch (std::exception &e) {
	        status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelIgnoredIntegratorParam == pasynUser->reason) {
    *value = 0;
    int integrator = pasynUser->timeout;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->ignoredIntegrator[integrator]) {
          *value = 1;
        }
      } catch (std::exception &e) {
	        status = asynError;
      }
    }
  }
  else if (_mpsDigitalChannelLatchedParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->digitalChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        *value = Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->latchedValue;
      } catch (std::exception &e) {
	      status = asynError;
      }
    }
  }
  else if (_mpsAppActiveEnableParam == pasynUser->reason) {
    if (addr >= FW_NUM_APPLICATION_MASKS)
      return asynError;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->applicationCards->find(addr) ==
            Engine::getInstance().getCurrentDb()->applicationCards->end()) {
              LOG_TRACE("DRIVER", "ERROR: ApplicationCard not found, key=" << addr);
              std::cout << "*** ERROR: ApplicationCard not found, key=" << addr << std::endl;
              return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->active) {
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
  else if (_mpsDigitalChannelBypassStatusParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->digitalChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->digitalChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: DigitalChannel not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->bypass->status == BYPASS_VALID) {
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
  else if (_mpsFaultBypassStatusParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->faults->find(addr) ==
            Engine::getInstance().getCurrentDb()->faults->end()) {
          LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->faults->at(addr)->bypass->status == BYPASS_VALID) {
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
  else if (_mpsAppBypassStatusParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->applicationCards->find(addr) ==
            Engine::getInstance().getCurrentDb()->applicationCards->end()) {
          LOG_TRACE("DRIVER", "ERROR: Fault not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->bypassed) {
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
  else if (_mpsAnalogChannelLatchedParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->analogChannels->find(addr) ==
            Engine::getInstance().getCurrentDb()->analogChannels->end()) {
          LOG_TRACE("DRIVER", "ERROR: AnalogChannel not found, key=" << addr);
          return asynError;
        }
        // mask
        // Non-zero *value means threshold exceeded
        *value = Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->latchedValue & mask;
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
  else if (_mpsIgnoreConditionParam == pasynUser->reason) {
    *value = 0;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->ignoreConditions->find(addr) ==
            Engine::getInstance().getCurrentDb()->ignoreConditions->end()) {
          LOG_TRACE("DRIVER", "ERROR: ignoreCondition not found, key=" << addr);
          return asynError;
        }
        if (Engine::getInstance().getCurrentDb()->ignoreConditions->at(addr)->state) {
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

  if (_mpsMitigationUnlatchParam == pasynUser->reason) {
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
  else if (_mpsBeamFaultClearParam == pasynUser->reason) {
    Firmware::getInstance().beamFaultClear();
    return status;
  }
  else if (_mpsFwResetAll == pasynUser->reason) {
    Firmware::getInstance().beamFaultClear(); 
    Firmware::getInstance().evalLatchClear(); 
    Firmware::getInstance().monErrClear();
    Firmware::getInstance().swErrClear();
    Firmware::getInstance().moConcErrClear();
    Firmware::getInstance().toErrClear();
    if ( Engine::getInstance().unlatchAllowed() ){
      {
        std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
        Engine::getInstance().getCurrentDb()->unlatchAll();
      }
      Engine::getInstance().clearSoftwareLatch();
    }
    return status;
  }
  else if (_mpsAppTimeoutEnableParam == pasynUser->reason) {
    if (addr >= FW_NUM_APPLICATION_MASKS)
      return asynError;

    Firmware::getInstance().setAppTimeoutEnable(addr, !!value);
    return status;
  }
  if (!Engine::getInstance().isInitialized()) {
    // Database has not been loaded
    LOG_TRACE("DRIVER", "ERROR: Database not initialized (reason=" << pasynUser->reason
	      << ", addr=" << addr << ")");
    status = setUIntDigitalParam(addr, pasynUser->reason, value, mask);
    return status;
  }

  if (_mpsDigitalChannelUnlatchParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->unlatch();
        status = setUIntDigitalParam(addr, pasynUser->reason,
                  Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->latchedValue, mask);
        LOG_TRACE("DRIVER", "Unlatch: "
            << Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->name
            << " value: " << Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->latchedValue);
      } catch (std::exception &e) {
	      status = asynError;
      }
    }
  }
  else if (_mpsDigitalChannelUnlatchFastParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->unlatch();
        status = setUIntDigitalParam(addr, pasynUser->reason,
                  Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->latchedValue, mask);
        LOG_TRACE("DRIVER", "Unlatch: "
            << Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->name
            << " value: " << Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->latchedValue);
      } catch (std::exception &e) {
	      status = asynError;
      }
    Firmware::getInstance().evalLatchClear();
    }
  }
  else if (_mpsAppActiveEnableParam == pasynUser->reason) {
    if (addr >= FW_NUM_APPLICATION_MASKS)
      return asynError;
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        if (Engine::getInstance().getCurrentDb()->applicationCards->find(addr) ==
            Engine::getInstance().getCurrentDb()->applicationCards->end()) {
              LOG_TRACE("DRIVER", "ERROR: ApplicationCard not found, key=" << addr);
              std::cout << "*** ERROR: ApplicationCard not found, key=" << addr << std::endl;
              return asynError;
        }
        Engine::getInstance().getCurrentDb()->applicationCards->at(addr)->active = !!value;
      } catch (std::exception &e) {
        status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelUnlatchParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        uint32_t latchedValue = Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->unlatch(mask);

        status = setUIntDigitalParam(addr, pasynUser->reason, latchedValue, mask);
        LOG_TRACE("DRIVER", "Unlatch: "
            << Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->channel->name
            << " latchedValue: " << latchedValue << ", unlatched: "
            << Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->latchedValue
            << " mask: " << mask);
      } catch (std::exception &e) {
	      status = asynError;
      }
    Firmware::getInstance().evalLatchClear();
    }
  }
  else if (_mpsDigitalChannelBypassValueParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->bypass->value = value;
        LOG_TRACE("DRIVER", "BypassValue: "
            << Engine::getInstance().getCurrentDb()->digitalChannels->at(addr)->name
            << " value: " << value);
      } catch (std::exception &e) {
	      status = asynError;
      }
    }
  }
  else if (_mpsAnalogChannelBypassValueParam == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      try {
        Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->bypass[bitIndex]->value = value;
        LOG_TRACE("DRIVER", "BypassValue: "
            << Engine::getInstance().getCurrentDb()->analogChannels->at(addr)->channel->name
            << " value: " << value << " (threshold=" << bitIndex << ")");
      } catch (const std::out_of_range &e) {
	      LOG_TRACE("DRIVER", "ERROR: AnalogChannels out of range, key=" << addr);
      }
    }
  }
  else if (_mpsUpdateClearParam == pasynUser->reason) {
    Engine::getInstance().clearMaxTimers();
    return status;
  }
  else if (_mpsForceDestBeamClass == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      if (value != 0) {
	      Engine::getInstance().getCurrentDb()->forceBeamDestination(addr, value);
      }
      else {
	      Engine::getInstance().getCurrentDb()->forceBeamDestination(addr, CLEAR_BEAM_CLASS);
      }
    }
    return status;
  }
  else if (_mpsSoftPermitDestBeamClass == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      if (value != 0) {
	      Engine::getInstance().getCurrentDb()->softPermitDestination(addr, value);
      }
      else {
	      Engine::getInstance().getCurrentDb()->softPermitDestination(addr, CLEAR_BEAM_CLASS);
      }
    }
    return status;
  }
  else if (_mpsMaxPermitDestBeamClass == pasynUser->reason) {
    {
      std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
      if (value != 0) {
	      Engine::getInstance().getCurrentDb()->setMaxPermit(value);
      }
      else {
	      Engine::getInstance().getCurrentDb()->setMaxPermit(CLEAR_BEAM_CLASS);
      }
    }
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
  } catch (DbException &ex) {
    std::cerr << "ERROR: Exception loading config " << fullName << std::endl;
    std::cerr << ex.what() << std::endl;
    return asynError;
  } catch (CentralNodeException &ex) {
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
    } catch (std::exception &ex) {
      std::cerr << "ERROR: Invalid database info" << std::endl;
      return asynError;
    }
  }

  // Make sure the beamDestinationId and beamClassId for setting Linac to PC0 via PV is correct
  {
    std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
    DbBeamClassMap::iterator beamClassIt = Engine::getInstance().getCurrentDb()->beamClasses->find(1);
    if (beamClassIt == Engine::getInstance().getCurrentDb()->beamClasses->end()) {
      std::cerr << "ERROR: Failed to find BeamClass with ID=1 (Beam Off), please check MPS database." << std::endl;
      return asynError;
    }
    else {
      if ((*beamClassIt).second->name != "Beam Off") {
        std::cerr << "ERROR: BeamClass with ID=1 is does not have name 'Beam Off', please check MPS database." << std::endl;
        return asynError;
      }
    }
    DbBeamDestinationMap::iterator beamDestIt = Engine::getInstance().getCurrentDb()->beamDestinations->find(3);
    if (beamDestIt == Engine::getInstance().getCurrentDb()->beamDestinations->end()) {
      std::cerr << "ERROR: Failed to find BeamDestination with ID=3 (SC_BSYD), please check MPS database." << std::endl;
      return asynError;
    }
    else {
      if ((*beamDestIt).second->name != "SC_BSYD") {
        std::cerr << "ERROR: BeamDestination with ID=3 is does not have name 'SC_BSYD', please check MPS database." << std::endl;
        return asynError;
      }
    }
  }
  //unlatch all faults
  std::unique_lock<std::mutex> lock(*Engine::getInstance().getCurrentDb()->getMutex());
  Engine::getInstance().getCurrentDb()->unlatchAll();
  Engine::getInstance().clearSoftwareLatch();
  Firmware::getInstance().evalLatchClear();
  return asynSuccess;
}

asynStatus CentralNodeDriver::loadTestDigitalChannels(const char *testFilename) {
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
      int digitalId;
      int digitalValue;
      testInputFile >> digitalId;
      testInputFile >> digitalValue;

      try {
	      Engine::getInstance().getCurrentDb()->digitalChannels->at(digitalId)->update(digitalValue);
      } catch (std::exception &ex) {
        std::cerr << "ERROR: Invalid digital channel ID: " << digitalId << std::endl;
        status = asynError;
      }
    }
  }

  return status;
}

asynStatus CentralNodeDriver::loadTestAnalogChannels(const char *testFilename) {
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
      int channelId;
      int channelValue;
      testInputFile >> channelId;
      testInputFile >> channelValue;

      try {
	      Engine::getInstance().getCurrentDb()->analogChannels->at(channelId)->update(channelValue);
      } catch (std::exception &ex) {
        std::cerr << "ERROR: Invalid analog channel ID: " << channelId << std::endl;
        status = asynError;
      }
    }
  }

  return status;
}

int CentralNodeDriver::getParamStringOffset(int deviceIndex, int thresholdIndex) {
  int totalDevices = Engine::getInstance().getCurrentDb()->getTotalDeviceCount();
  return deviceIndex + (totalDevices * thresholdIndex);
}

/**
 * @param expirationTime bypass expiration time in seconds since now.
 * If expirationTime is zero or negative the bypass is cancelled.
 */
asynStatus CentralNodeDriver::setBypass(BypassType bypassType, int id,
					int thresholdIndex, epicsInt32 expirationTime) {
  asynStatus status = asynSuccess;
  time_t now;
  time(&now);

  LOG_TRACE("DRIVER", "Set bypass for channel " << id << ", thresholdIndex=" << thresholdIndex);

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
      if (bypassType == BYPASS_APPLICATION) {
        Engine::getInstance().getBypassManager()->setBypass(bypassType,
                        id, 0, expirationTime);
      }
      else if (bypassType == BYPASS_FAULT) {
        uint32_t bypassValue = Engine::getInstance().getCurrentDb()->faults->at(id)->bypass->value;
        Engine::getInstance().getBypassManager()->bypassFault(
                        id, bypassValue, expirationTime);
      }
      else if (bypassType == BYPASS_DIGITAL) {
        uint32_t bypassValue = Engine::getInstance().getCurrentDb()->digitalChannels->at(id)->bypass->value;
        Engine::getInstance().getBypassManager()->setBypass(bypassType,
                        id, bypassValue, expirationTime);
      }
      else {
        uint32_t bypassValue = Engine::getInstance().getCurrentDb()->analogChannels->at(id)->bypass[thresholdIndex]->value;
        Engine::getInstance().getBypassManager()->setThresholdBypass(bypassType,
                          id, bypassValue, expirationTime, thresholdIndex);
      }
    } catch (std::exception &e) {
      std::cerr << "ERROR: Exception while setting bypass:" << std::endl;
      std::cerr << e.what() << std::endl;
      return asynError;
    }
  }

  if (expirationTime == 0) {
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(id,
			      _mpsDigitalChannelBypassExpirationDateStringParam, "Not Bypassed");
    }
    else if (bypassType == BYPASS_ANALOG) {
      status = setStringParam(getParamStringOffset(id, thresholdIndex),
			      _mpsAnalogChannelBypassExpirationDateStringParam, "Not Bypassed");
    }
    else if (bypassType == BYPASS_FAULT) {
      status = setStringParam(id,
			      _mpsFaultBypassExpirationDateStringParam, "Not Bypassed");
    }
    else {
      status = setStringParam(id,
			      _mpsAppBypassExpirationDateStringParam, "Not Bypassed");
    }
  }
  else {
    time_t expTime = expirationTime;
    if (bypassType == BYPASS_DIGITAL) {
      status = setStringParam(id,
			      _mpsDigitalChannelBypassExpirationDateStringParam, ctime(&expTime));
    }
    else if (bypassType == BYPASS_ANALOG) {
      status = setStringParam(getParamStringOffset(id, thresholdIndex),
			      _mpsAnalogChannelBypassExpirationDateStringParam, ctime(&expTime));
    }
    else if (bypassType == BYPASS_FAULT) {
      status = setStringParam(id,
            _mpsFaultBypassExpirationDateStringParam, ctime(&expTime));
    }
    else {
      status = setStringParam(id,
            _mpsAppBypassExpirationDateStringParam, ctime(&expTime));
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

void CentralNodeDriver::setTestMode(int mode) {
  _testMode = mode;
}

int CentralNodeDriver::getTestMode() {
  return _testMode;
}

void CentralNodeDriver::report(FILE *fp, int reportDetails) {
  fprintf(fp, "%s: driver report\n", portName);

  std::ostringstream details;

  if (reportDetails > 2) {
    details << "\n================== MKSU Parameters ====================\n";
  }

  asynPortDriver::report(fp, reportDetails);
}

