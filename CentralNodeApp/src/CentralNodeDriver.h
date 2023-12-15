#ifndef CENTRALNODEDRIVER_H
#define CENTRALNODEDRIVER_H

#include <string>
#include <map>
#include <stdio.h>

#include <central_node_bypass.h>
#include <central_node_firmware.h>
#include <asynPortDriver.h>

// Environment variables
#define MPS_ENV_CONFIG_PATH                        "MPS_ENV_CONFIG_PATH"
#define MPS_ENV_FW_CONFIG                          "MPS_ENV_FW_CONFIG"
#define MPS_ENV_HISTORY_HOST                       "MPS_ENV_HISTORY_HOST"
#define MPS_ENV_HISTORY_PORT                       "MPS_ENV_HISTORY_PORT"
#define MPS_ENV_UPDATE_TIMEOUT                     "MPS_ENV_UPDATE_TIMEOUT"

// List of ASYN parameter names
#define MPS_CONFIG_LOAD_STRING                     "MPS_CONFIG_LOAD"
#define MPS_DIGITAL_CHANNEL_STRING                 "MPS_DIGITAL_CHANNEL"
#define MPS_ANALOG_CHANNEL_STRING                  "MPS_ANALOG_CHANNEL"
#define MPS_SW_MITIGATION_STRING                   "MPS_SW_MITIGATION"
#define MPS_FAULT_STRING                           "MPS_FAULT"
#define MPS_FAULT_TEST_STRING                      "MPS_FAULT_TEST"
#define MPS_FAULT_FAULTED_STRING                   "MPS_FAULT_FAULTED"
#define MPS_FAULT_ACTIVE_STRING                    "MPS_FAULT_ACTIVE"
#define MPS_FAULT_IGNORED_STRING                   "MPS_FAULT_IGNORED"
#define MPS_FAULT_LATCHED_STRING                   "MPS_FAULT_LATCHED"
#define MPS_FAULT_UNLATCH_STRING                   "MPS_FAULT_UNLATCH"
#define MPS_FAULT_BYPV_STRING                      "MPS_FAULT_BYPV"
#define MPS_FAULT_BYPS_STRING                      "MPS_FAULT_BYPS"
#define MPS_FAULT_BYPEXPDATE_STRING                "MPS_FAULT_BYPEXPDATE"
#define MPS_FAULT_REMAINING_BYPTIME_STRING         "MPS_FAULT_REMAINING_BYPTIME"
#define MPS_FAULT_BYPEXPDATE_STRING_STRING         "MPS_FAULT_BYPEXPDATE_STRING"
#define MPS_DIGITAL_CHANNEL_LATCHED_STRING         "MPS_DIGITAL_CHANNEL_LATCHED"
#define MPS_DIGITAL_CHANNEL_UNLATCH_STRING         "MPS_DIGITAL_CHANNEL_UNLATCH"
#define MPS_DIGITAL_CHANNEL_UNLATCH_FAST_STRING    "MPS_DIGITAL_CHANNEL_FAST_UNLATCH"
#define MPS_DIGITAL_CHANNEL_BYPV_STRING            "MPS_DIGITAL_CHANNEL_BYPV"
#define MPS_DIGITAL_CHANNEL_BYPS_STRING            "MPS_DIGITAL_CHANNEL_BYPS"
#define MPS_DIGITAL_CHANNEL_BYPEXPDATE_STRING      "MPS_DIGITAL_CHANNEL_BYPEXPDATE"
#define MPS_DIGITAL_CHANNEL_REMAINING_BYPTIME_STRING "MPS_DIGITAL_CHANNEL_REMAINING_BYPTIME"
#define MPS_DIGITAL_CHANNEL_BYPEXPDATE_STRING_STRING "MPS_DIGITAL_CHANNEL_BYPEXPDATE_STRING"
#define MPS_ANALOG_CHANNEL_LATCHED_STRING           "MPS_ANALOG_CHANNEL_LATCHED"
#define MPS_ANALOG_CHANNEL_UNLATCH_STRING           "MPS_ANALOG_CHANNEL_UNLATCH"
#define MPS_ANALOG_CHANNEL_BYPV_STRING              "MPS_ANALOG_CHANNEL_BYPV"
#define MPS_ANALOG_CHANNEL_BYPS_STRING              "MPS_ANALOG_CHANNEL_BYPS"
#define MPS_ANALOG_CHANNEL_BYPEXPDATE_STRING        "MPS_ANALOG_CHANNEL_BYPEXPDATE"
#define MPS_ANALOG_CHANNEL_REMAINING_BYPTIME_STRING "MPS_ANALOG_CHANNEL_REMAINING_BYPTIME"
#define MPS_ANALOG_CHANNEL_BYPEXPDATE_STRING_STRING "MPS_ANALOG_CHANNEL_BYPEXPDATE_STRING"
#define MPS_ANALOG_CHANNEL_IGNORED_STRING           "MPS_ANALOG_CHANNEL_IGNORED"
#define MPS_ANALOG_CHANNEL_IGNORED_INTEGRATOR_STRING "MPS_ANALOG_CHANNEL_IGNORED_INTEGRATOR"
#define MPS_UNLATCH_ALL_STRING                     "MPS_UNLATCH_ALL"
#define MPS_FW_BUILD_STAMP_STRING_STRING           "MPS_FW_BUILD_STAMP_STRING"
#define MPS_SW_UPDATE_RATE_STRING                  "MPS_SW_UPDATE_RATE"
#define MPS_FW_MITIGATION_STRING                   "MPS_FW_MITIGATION"
#define MPS_MITIGATION_STRING                      "MPS_MITIGATION"
#define MPS_SW_UPDATE_COUNTER_STRING               "MPS_SW_UPDATE_COUNTER"
#define MPS_ENGINE_START_TIME_STRING_STRING        "MPS_ENGINE_START_TIME_STRING"
#define MPS_LATCHED_MITIGATION_STRING              "MPS_LATCHED_MITIGATION"
#define MPS_MITIGATION_UNLATCH_STRING              "MPS_MITIGATION_UNLATCH"
#define MPS_APP_BYPS_STRING                        "MPS_APP_BYPS"
#define MPS_APP_BYPEXPDATE_STRING                  "MPS_APP_BYPEXPDATE"
#define MPS_APP_REMAINING_BYPTIME_STRING           "MPS_APP_REMAINING_BYPTIME"
#define MPS_APP_BYPEXPDATE_STRING_STRING           "MPS_APP_BYPEXPDATE_STRING"
#define MPS_APP_STATUS_STRING                      "MPS_APP_STATUS"
#define MPS_APP_TIMESTAMP_LOW_BITS_STRING          "MPS_APP_TIMESTAMP_LOW_BITS"
#define MPS_APP_TIMESTAMP_HIGH_BITS_STRING         "MPS_APP_TIMESTAMP_HIGH_BITS"
#define MPS_MON_ERR_CLEAR_STRING                   "MPS_MON_ERR_CLEAR"
#define MPS_SW_ERR_CLEAR_STRING                    "MPS_SW_ERR_CLEAR"
#define MPS_TO_ERR_CLEAR_STRING                    "MPS_TO_ERR_CLEAR"
#define MPS_MO_CONC_ERR_CLEAR_STRING               "MPS_MO_CONC_ERR_CLEAR"
#define MPS_BEAM_FAULT_CLEAR_STRING                "MPS_BEAM_FAULT_CLEAR"
#define MPS_EVALCYCLE_AVG_TIME_STRING              "MPS_EVALCYCLE_AVG_TIME"
#define MPS_EVALCYCLE_MAX_TIME_STRING              "MPS_EVALCYCLE_MAX_TIME"
#define MPS_EVAL_AVG_TIME_STRING                   "MPS_EVAL_AVG_TIME"
#define MPS_EVAL_MAX_TIME_STRING                   "MPS_EVAL_MAX_TIME"
#define MPS_UPDATE_AVG_TIME_STRING                 "MPS_UPDATE_AVG_TIME"
#define MPS_UPDATE_MAX_TIME_STRING                 "MPS_UPDATE_MAX_TIME"
#define MPS_UPDATE_TIME_CLEAR_STRING               "MPS_UPDATE_TIME_CLEAR"
#define MPS_FWUPDATE_AVG_PERIOD_STRING             "MPS_FWUPDATE_AVG_PERIOD"
#define MPS_FWUPDATE_MAX_PERIOD_STRING             "MPS_FWUPDATE_MAX_PERIOD"
#define MPS_WDUPDATE_AVG_PERIOD_STRING             "MPS_WDUPDATE_AVG_PERIOD"
#define MPS_WDUPDATE_MAX_PERIOD_STRING             "MPS_WDUPDATE_MAX_PERIOD"
#define MPS_CONFIG_DB_SRC_STRING                   "MPS_CONFIG_DB_SRC"
#define MPS_CONFIG_DB_USER_STRING                  "MPS_CONFIG_DB_USER"
#define MPS_CONFIG_DB_DATE_STRING                  "MPS_CONFIG_DB_DATE"
#define MPS_CONFIG_DB_MD5SUM_STRING                "MPS_CONFIG_DB_MD5SUM"
#define MPS_STATE_STRING                           "MPS_STATE"
#define MPS_IGNORE_CONDITION_STRING                "MPS_IGNORE_CONDITION"
#define MPS_FW_SOFTWARE_WDOG_COUNTER_STRING        "MPS_FW_SOFTWARE_WDOG_COUNTER"
#define MPS_FORCE_LINAC_PC0_STRING                 "MPS_FORCE_LINAC_PC0"
#define MPS_FORCE_AOM_PC0_STRING                   "MPS_FORCE_AOM_PC0"
#define MPS_FORCE_DEST_STRING                      "MPS_FORCE_DEST"
#define MPS_SW_PERMIT_DEST_STRING                  "MPS_SW_PERMIT_DEST"
#define MPS_FINAL_MITIGATION_STRING                "MPS_FINAL_BC"
#define MPS_FW_RESET_ALL_STRING                    "MPS_FW_RESET_ALL"
#define MPS_TIMING_BC_STRING                       "MPS_TIMING_BC"
#define MPS_MAX_BEAM_CLASS_STRING                  "MPS_MAX_BEAM_CLASS"
#define MPS_TEST_MODE_STRING                       "MPS_TEST_MODE"

#define TEST_DIGITAL_CHANNEL_STRING                "TEST_DIGITAL_CHANNEL"
#define TEST_ANALOG_CHANNEL_STRING                 "TEST_ANALOG_CHANNEL"
#define TEST_CHECK_FAULTS_STRING                   "TEST_CHECK_FAULTS"
#define TEST_CHECK_BYPASS_STRING                   "TEST_CHECK_BYPASS"

#define MPS_APP_TIMEOUT_ENABLE_STRING              "MPS_APP_TIMEOUT_ENABLE"
#define MPS_APP_ACTIVE_ENABLE_STRING               "MPS_APP_ACTIVE_ENABLE"


enum CentralNodeState {
  MPS_STATE_IDLE,
  MPS_STATE_RUNNING,
  MPS_STATE_LOAD_CONFIG,
  MPS_STATE_LOAD_CONFIG_ERROR,
};

class CentralNodeDriver : public asynPortDriver {
public:
  CentralNodeDriver(const char *portname, std::string configPath,
		    std::string historyServer, int historyPort);
  virtual ~CentralNodeDriver();

  /* virtual asynStatus drvUserCreate(asynUser *pasynUser, const char *drvInfo, const char **ppTypeName, */
  /* 				   size_t *psize); */
  //virtual asynStatus readOctet(asynUser *pasynUser, char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t maxChars, size_t *nActual);
  virtual asynStatus readInt32(asynUser *pasynUser, epicsInt32 *value);
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus readUInt32Digital(asynUser *pasynUser, epicsUInt32 *value, epicsUInt32 mask);
  virtual asynStatus writeUInt32Digital(asynUser *pasynUser, epicsUInt32 value, epicsUInt32 mask);

  virtual void report(FILE *fp, int details);

  void printQueue();
  void testLoadConfig(std::string yamlFile);
  void showMitigation();
  void showFaults();
  void showFirmware();
  void showDatabaseInfo();
  void showEngineInfo();

  void setTestMode(int mode);
  int getTestMode();

 private:
  std::string _configPath;
  uint32_t _inputUpdateTimeout;

  // List of ASYN parameters
  int _mpsConfigLoadParam;
  int _mpsDigitalChannelParam;
  int _mpsAnalogChannelParam;
  int _mpsSwMitigationParam;
  int _mpsFaultParam;
  int _mpsFaultParamTest;
  int _mpsFaultFaultedParam;
  int _mpsFaultActiveParam;
  int _mpsFaultIgnoredParam;
  int _mpsFaultLatchedParam;
  int _mpsFaultUnlatchParam;
  int _mpsFaultBypassValueParam;
  int _mpsFaultBypassStatusParam;
  int _mpsFaultBypassExpirationDateParam;
  int _mpsFaultBypassRemainingExpirationTimeParam;
  int _mpsFaultBypassExpirationDateStringParam;
  int _mpsDigitalChannelLatchedParam;
  int _mpsDigitalChannelUnlatchParam;
  int _mpsDigitalChannelUnlatchFastParam;
  int _mpsDigitalChannelBypassValueParam;
  int _mpsDigitalChannelBypassStatusParam;
  int _mpsDigitalChannelBypassExpirationDateParam;
  int _mpsDigitalChannelBypassRemainingExpirationTimeParam;
  int _mpsDigitalChannelBypassExpirationDateStringParam;
  int _mpsAnalogChannelLatchedParam;
  int _mpsAnalogChannelUnlatchParam;
  int _mpsAnalogChannelBypassValueParam;
  int _mpsAnalogChannelBypassStatusParam;
  int _mpsAnalogChannelBypassExpirationDateParam;
  int _mpsAnalogChannelBypassRemainingExpirationTimeParam;
  int _mpsAnalogChannelBypassExpirationDateStringParam;
  int _mpsAnalogChannelIgnoredParam;
  int _mpsAnalogChannelIgnoredIntegratorParam;
  int _mpsUnlatchAllParam;
  int _mpsFwBuildStampParam;
  int _mpsUpdateRateParam;
  int _mpsFwMitigationParam;
  int _mpsMitigationParam;
  int _mpsUpdateCounterParam;
  int _mpsEngineStartTimeStringParam;
  int _mpsLatchedMitigationParam;
  int _mpsFinalBeamClassParam;
  int _mpsMitigationUnlatchParam;
  int _mpsAppBypassStatusParam;
  int _mpsAppBypassExpirationDateParam;
  int _mpsAppBypassRemainingExpirationTimeParam;
  int _mpsAppBypassExpirationDateStringParam;
  int _mpsAppStatusParam;
  int _mpsAppTimestampLowBitsParam;
  int _mpsAppTimestampHighBitsParam;
  int _mpsMonErrClearParam;
  int _mpsSwErrClearParam;
  int _mpsToErrClearParam;
  int _mpsMoConcErrClearParam;
  int _mpsBeamFaultClearParam;
  int _mpsEvalAvgParam;
  int _mpsEvalMaxParam;
  int _mpsEvalCycleAvgParam;
  int _mpsEvalCycleMaxParam;
  int _mpsUpdateAvgParam;
  int _mpsUpdateMaxParam;
  int _mpsUpdateClearParam;
  int _mpsFwUpdateAvgParam;
  int _mpsFwUpdateMaxParam;
  int _mpsWdUpdateAvgParam;
  int _mpsWdUpdateMaxParam;
  int _mpsConfigDbSourceParam;
  int _mpsConfigDbUserParam;
  int _mpsConfigDbDateParam;
  int _mpsConfigDbMd5SumParam;
  int _mpsStateParam;
  int _mpsIgnoreConditionParam;
  int _mpsFwSoftwareWdogCounterParam;
  int _mpsForceLinacPc0Param;
  int _mpsForceAomPc0Param;
  int _mpsForceDestBeamClass;
  int _mpsSoftPermitDestBeamClass;
  int _mpsFwResetAll;
  int _mpsTimingBCparam;
  int _mpsMaxPermitDestBeamClass;
  int _mpsTestMode;

  int _testDigitalChannelParam;
  int _testAnalogChannelParam;
  int _testCheckFaultsParam;
  int _testCheckBypassParam;

  int _mpsAppTimeoutEnableParam;
  int _mpsAppActiveEnableParam;

  int _testMode;

  //  CentralNodeState _mpsState;

  asynStatus loadConfig(const char *config);
  asynStatus loadTestDigitalChannels(const char *testFileName);
  asynStatus loadTestAnalogChannels(const char *testFileName);

  asynStatus setBypass(BypassType bypassType, int channelId, int thresholdIndex, epicsInt32 expirationTime);

  int getParamStringOffset(int deviceIndex, int thresholdIndex);
};

#endif
