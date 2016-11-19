/* THIS IS A GENERATED FILE. DO NOT EDIT! */
/* Generated from ../O.Common/CentralNode.dbd */

#include <string.h>

#include "epicsStdlib.h"
#include "iocsh.h"
#include "iocshRegisterCommon.h"
#include "registryCommon.h"

extern "C" {

epicsShareExtern rset *pvar_rset_aaiRSET;
epicsShareExtern int (*pvar_func_aaiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aaoRSET;
epicsShareExtern int (*pvar_func_aaoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aiRSET;
epicsShareExtern int (*pvar_func_aiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aoRSET;
epicsShareExtern int (*pvar_func_aoRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_aSubRSET;
epicsShareExtern int (*pvar_func_aSubRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_biRSET;
epicsShareExtern int (*pvar_func_biRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_boRSET;
epicsShareExtern int (*pvar_func_boRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcRSET;
epicsShareExtern int (*pvar_func_calcRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_calcoutRSET;
epicsShareExtern int (*pvar_func_calcoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_compressRSET;
epicsShareExtern int (*pvar_func_compressRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_dfanoutRSET;
epicsShareExtern int (*pvar_func_dfanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_eventRSET;
epicsShareExtern int (*pvar_func_eventRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_fanoutRSET;
epicsShareExtern int (*pvar_func_fanoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longinRSET;
epicsShareExtern int (*pvar_func_longinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longoutRSET;
epicsShareExtern int (*pvar_func_longoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiRSET;
epicsShareExtern int (*pvar_func_mbbiRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbbiDirectRSET;
epicsShareExtern int (*pvar_func_mbbiDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboRSET;
epicsShareExtern int (*pvar_func_mbboRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_mbboDirectRSET;
epicsShareExtern int (*pvar_func_mbboDirectRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_permissiveRSET;
epicsShareExtern int (*pvar_func_permissiveRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_selRSET;
epicsShareExtern int (*pvar_func_selRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_seqRSET;
epicsShareExtern int (*pvar_func_seqRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stateRSET;
epicsShareExtern int (*pvar_func_stateRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringinRSET;
epicsShareExtern int (*pvar_func_stringinRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_stringoutRSET;
epicsShareExtern int (*pvar_func_stringoutRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subRSET;
epicsShareExtern int (*pvar_func_subRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_subArrayRSET;
epicsShareExtern int (*pvar_func_subArrayRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_waveformRSET;
epicsShareExtern int (*pvar_func_waveformRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_bsaRSET;
epicsShareExtern int (*pvar_func_bsaRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_longSubRSET;
epicsShareExtern int (*pvar_func_longSubRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_erRSET;
epicsShareExtern int (*pvar_func_erRecordSizeOffset)(dbRecordType *pdbRecordType);
epicsShareExtern rset *pvar_rset_ereventRSET;
epicsShareExtern int (*pvar_func_ereventRecordSizeOffset)(dbRecordType *pdbRecordType);

static const char * const recordTypeNames[32] = {
    "aai",
    "aao",
    "ai",
    "ao",
    "aSub",
    "bi",
    "bo",
    "calc",
    "calcout",
    "compress",
    "dfanout",
    "event",
    "fanout",
    "longin",
    "longout",
    "mbbi",
    "mbbiDirect",
    "mbbo",
    "mbboDirect",
    "permissive",
    "sel",
    "seq",
    "state",
    "stringin",
    "stringout",
    "sub",
    "subArray",
    "waveform",
    "bsa",
    "longSub",
    "er",
    "erevent"
};

static const recordTypeLocation rtl[32] = {
    {pvar_rset_aaiRSET, pvar_func_aaiRecordSizeOffset},
    {pvar_rset_aaoRSET, pvar_func_aaoRecordSizeOffset},
    {pvar_rset_aiRSET, pvar_func_aiRecordSizeOffset},
    {pvar_rset_aoRSET, pvar_func_aoRecordSizeOffset},
    {pvar_rset_aSubRSET, pvar_func_aSubRecordSizeOffset},
    {pvar_rset_biRSET, pvar_func_biRecordSizeOffset},
    {pvar_rset_boRSET, pvar_func_boRecordSizeOffset},
    {pvar_rset_calcRSET, pvar_func_calcRecordSizeOffset},
    {pvar_rset_calcoutRSET, pvar_func_calcoutRecordSizeOffset},
    {pvar_rset_compressRSET, pvar_func_compressRecordSizeOffset},
    {pvar_rset_dfanoutRSET, pvar_func_dfanoutRecordSizeOffset},
    {pvar_rset_eventRSET, pvar_func_eventRecordSizeOffset},
    {pvar_rset_fanoutRSET, pvar_func_fanoutRecordSizeOffset},
    {pvar_rset_longinRSET, pvar_func_longinRecordSizeOffset},
    {pvar_rset_longoutRSET, pvar_func_longoutRecordSizeOffset},
    {pvar_rset_mbbiRSET, pvar_func_mbbiRecordSizeOffset},
    {pvar_rset_mbbiDirectRSET, pvar_func_mbbiDirectRecordSizeOffset},
    {pvar_rset_mbboRSET, pvar_func_mbboRecordSizeOffset},
    {pvar_rset_mbboDirectRSET, pvar_func_mbboDirectRecordSizeOffset},
    {pvar_rset_permissiveRSET, pvar_func_permissiveRecordSizeOffset},
    {pvar_rset_selRSET, pvar_func_selRecordSizeOffset},
    {pvar_rset_seqRSET, pvar_func_seqRecordSizeOffset},
    {pvar_rset_stateRSET, pvar_func_stateRecordSizeOffset},
    {pvar_rset_stringinRSET, pvar_func_stringinRecordSizeOffset},
    {pvar_rset_stringoutRSET, pvar_func_stringoutRecordSizeOffset},
    {pvar_rset_subRSET, pvar_func_subRecordSizeOffset},
    {pvar_rset_subArrayRSET, pvar_func_subArrayRecordSizeOffset},
    {pvar_rset_waveformRSET, pvar_func_waveformRecordSizeOffset},
    {pvar_rset_bsaRSET, pvar_func_bsaRecordSizeOffset},
    {pvar_rset_longSubRSET, pvar_func_longSubRecordSizeOffset},
    {pvar_rset_erRSET, pvar_func_erRecordSizeOffset},
    {pvar_rset_ereventRSET, pvar_func_ereventRecordSizeOffset}
};

epicsShareExtern dset *pvar_dset_devAaiSoft;
epicsShareExtern dset *pvar_dset_devAaoSoft;
epicsShareExtern dset *pvar_dset_devAiSoft;
epicsShareExtern dset *pvar_dset_devAiSoftRaw;
epicsShareExtern dset *pvar_dset_devTimestampAI;
epicsShareExtern dset *pvar_dset_devAiGeneralTime;
epicsShareExtern dset *pvar_dset_devAiStats;
epicsShareExtern dset *pvar_dset_devAiClusts;
epicsShareExtern dset *pvar_dset_devAoSoft;
epicsShareExtern dset *pvar_dset_devAoSoftRaw;
epicsShareExtern dset *pvar_dset_devAoSoftCallback;
epicsShareExtern dset *pvar_dset_devAoStats;
epicsShareExtern dset *pvar_dset_devAoBsa;
epicsShareExtern dset *pvar_dset_devBiSoft;
epicsShareExtern dset *pvar_dset_devBiSoftRaw;
epicsShareExtern dset *pvar_dset_devMrfErEpicsBi;
epicsShareExtern dset *pvar_dset_devBoSoft;
epicsShareExtern dset *pvar_dset_devBoSoftRaw;
epicsShareExtern dset *pvar_dset_devBoSoftCallback;
epicsShareExtern dset *pvar_dset_devBoGeneralTime;
epicsShareExtern dset *pvar_dset_devCalcoutSoft;
epicsShareExtern dset *pvar_dset_devCalcoutSoftCallback;
epicsShareExtern dset *pvar_dset_devEventSoft;
epicsShareExtern dset *pvar_dset_devMrfErEpicsEvent;
epicsShareExtern dset *pvar_dset_devLiSoft;
epicsShareExtern dset *pvar_dset_devLiGeneralTime;
epicsShareExtern dset *pvar_dset_devLoSoft;
epicsShareExtern dset *pvar_dset_devLoSoftCallback;
epicsShareExtern dset *pvar_dset_devMbbiSoft;
epicsShareExtern dset *pvar_dset_devMbbiSoftRaw;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoft;
epicsShareExtern dset *pvar_dset_devMbbiDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoft;
epicsShareExtern dset *pvar_dset_devMbboSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboSoftCallback;
epicsShareExtern dset *pvar_dset_devMbboDirectSoft;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftRaw;
epicsShareExtern dset *pvar_dset_devMbboDirectSoftCallback;
epicsShareExtern dset *pvar_dset_devSiSoft;
epicsShareExtern dset *pvar_dset_devTimestampSI;
epicsShareExtern dset *pvar_dset_devSiGeneralTime;
epicsShareExtern dset *pvar_dset_devStringinStats;
epicsShareExtern dset *pvar_dset_devStringinEnvVar;
epicsShareExtern dset *pvar_dset_devStringinEpics;
epicsShareExtern dset *pvar_dset_devSoSoft;
epicsShareExtern dset *pvar_dset_devSoSoftCallback;
epicsShareExtern dset *pvar_dset_devSoStdio;
epicsShareExtern dset *pvar_dset_devSASoft;
epicsShareExtern dset *pvar_dset_devWfSoft;
epicsShareExtern dset *pvar_dset_devWaveformStats;
epicsShareExtern dset *pvar_dset_devBsa;
epicsShareExtern dset *pvar_dset_devMrfEr;
epicsShareExtern dset *pvar_dset_devMrfErevent;

static const char * const deviceSupportNames[53] = {
    "devAaiSoft",
    "devAaoSoft",
    "devAiSoft",
    "devAiSoftRaw",
    "devTimestampAI",
    "devAiGeneralTime",
    "devAiStats",
    "devAiClusts",
    "devAoSoft",
    "devAoSoftRaw",
    "devAoSoftCallback",
    "devAoStats",
    "devAoBsa",
    "devBiSoft",
    "devBiSoftRaw",
    "devMrfErEpicsBi",
    "devBoSoft",
    "devBoSoftRaw",
    "devBoSoftCallback",
    "devBoGeneralTime",
    "devCalcoutSoft",
    "devCalcoutSoftCallback",
    "devEventSoft",
    "devMrfErEpicsEvent",
    "devLiSoft",
    "devLiGeneralTime",
    "devLoSoft",
    "devLoSoftCallback",
    "devMbbiSoft",
    "devMbbiSoftRaw",
    "devMbbiDirectSoft",
    "devMbbiDirectSoftRaw",
    "devMbboSoft",
    "devMbboSoftRaw",
    "devMbboSoftCallback",
    "devMbboDirectSoft",
    "devMbboDirectSoftRaw",
    "devMbboDirectSoftCallback",
    "devSiSoft",
    "devTimestampSI",
    "devSiGeneralTime",
    "devStringinStats",
    "devStringinEnvVar",
    "devStringinEpics",
    "devSoSoft",
    "devSoSoftCallback",
    "devSoStdio",
    "devSASoft",
    "devWfSoft",
    "devWaveformStats",
    "devBsa",
    "devMrfEr",
    "devMrfErevent"
};

static const dset * const devsl[53] = {
    pvar_dset_devAaiSoft,
    pvar_dset_devAaoSoft,
    pvar_dset_devAiSoft,
    pvar_dset_devAiSoftRaw,
    pvar_dset_devTimestampAI,
    pvar_dset_devAiGeneralTime,
    pvar_dset_devAiStats,
    pvar_dset_devAiClusts,
    pvar_dset_devAoSoft,
    pvar_dset_devAoSoftRaw,
    pvar_dset_devAoSoftCallback,
    pvar_dset_devAoStats,
    pvar_dset_devAoBsa,
    pvar_dset_devBiSoft,
    pvar_dset_devBiSoftRaw,
    pvar_dset_devMrfErEpicsBi,
    pvar_dset_devBoSoft,
    pvar_dset_devBoSoftRaw,
    pvar_dset_devBoSoftCallback,
    pvar_dset_devBoGeneralTime,
    pvar_dset_devCalcoutSoft,
    pvar_dset_devCalcoutSoftCallback,
    pvar_dset_devEventSoft,
    pvar_dset_devMrfErEpicsEvent,
    pvar_dset_devLiSoft,
    pvar_dset_devLiGeneralTime,
    pvar_dset_devLoSoft,
    pvar_dset_devLoSoftCallback,
    pvar_dset_devMbbiSoft,
    pvar_dset_devMbbiSoftRaw,
    pvar_dset_devMbbiDirectSoft,
    pvar_dset_devMbbiDirectSoftRaw,
    pvar_dset_devMbboSoft,
    pvar_dset_devMbboSoftRaw,
    pvar_dset_devMbboSoftCallback,
    pvar_dset_devMbboDirectSoft,
    pvar_dset_devMbboDirectSoftRaw,
    pvar_dset_devMbboDirectSoftCallback,
    pvar_dset_devSiSoft,
    pvar_dset_devTimestampSI,
    pvar_dset_devSiGeneralTime,
    pvar_dset_devStringinStats,
    pvar_dset_devStringinEnvVar,
    pvar_dset_devStringinEpics,
    pvar_dset_devSoSoft,
    pvar_dset_devSoSoftCallback,
    pvar_dset_devSoStdio,
    pvar_dset_devSASoft,
    pvar_dset_devWfSoft,
    pvar_dset_devWaveformStats,
    pvar_dset_devBsa,
    pvar_dset_devMrfEr,
    pvar_dset_devMrfErevent
};

epicsShareExtern drvet *pvar_drvet_drvEvr;
epicsShareExtern drvet *pvar_drvet_drvMrf200Er;

static const char *driverSupportNames[2] = {
    "drvEvr",
    "drvMrf200Er"
};

static struct drvet *drvsl[2] = {
    pvar_drvet_drvEvr,
    pvar_drvet_drvMrf200Er
};

epicsShareExtern void (*pvar_func_initCentralNodeRegistrar)(void);
epicsShareExtern void (*pvar_func_asSub)(void);
epicsShareExtern void (*pvar_func_save_restoreRegister)(void);
epicsShareExtern void (*pvar_func_dbrestoreRegister)(void);
epicsShareExtern void (*pvar_func_asInitHooksRegister)(void);
epicsShareExtern void (*pvar_func_configMenuRegistrar)(void);
epicsShareExtern void (*pvar_func_caPutLogRegister)(void);
epicsShareExtern void (*pvar_func_cexpRegistrar)(void);
epicsShareExtern void (*pvar_func_iocshSystemCommand)(void);
epicsShareExtern void (*pvar_func_drvEvrRegister)(void);
epicsShareExtern void (*pvar_func_drvMrfErRegister)(void);
epicsShareExtern void (*pvar_func_drvMrfRegister)(void);
epicsShareExtern void (*pvar_func_evrEEPROMFixupRegister)(void);
epicsShareExtern void (*pvar_func_register_func_rebootProc)(void);
epicsShareExtern void (*pvar_func_register_func_scanMonInit)(void);
epicsShareExtern void (*pvar_func_register_func_scanMon)(void);
epicsShareExtern void (*pvar_func_register_func_evrTimeProcInit)(void);
epicsShareExtern void (*pvar_func_register_func_evrTimeProc)(void);
epicsShareExtern void (*pvar_func_register_func_evrTimeDiag)(void);
epicsShareExtern void (*pvar_func_register_func_evrTimeRate)(void);
epicsShareExtern void (*pvar_func_register_func_evrPatternProcInit)(void);
epicsShareExtern void (*pvar_func_register_func_evrPatternProc)(void);
epicsShareExtern void (*pvar_func_register_func_evrPatternState)(void);
epicsShareExtern void (*pvar_func_register_func_evrPatternSim)(void);
epicsShareExtern void (*pvar_func_register_func_evrPatternSimTest)(void);
epicsShareExtern void (*pvar_func_register_func_lsubTrigSelInit)(void);
epicsShareExtern void (*pvar_func_register_func_lsubTrigSel)(void);
epicsShareExtern void (*pvar_func_register_func_lsubEvSelInit)(void);
epicsShareExtern void (*pvar_func_register_func_lsubEvSel)(void);
epicsShareExtern void (*pvar_func_register_func_aSubEvOffsetInit)(void);
epicsShareExtern void (*pvar_func_register_func_aSubEvOffset)(void);
epicsShareExtern void (*pvar_func_register_func_aSubEvDescInit)(void);
epicsShareExtern void (*pvar_func_register_func_aSubEvDesc)(void);
epicsShareExtern void (*pvar_func_register_func_evrModifier5)(void);
epicsShareExtern void (*pvar_func_register_func_evrModifier5Bits)(void);

epicsShareExtern int *pvar_int_asCaDebug;
epicsShareExtern int *pvar_int_dbRecordsOnceOnly;
epicsShareExtern int *pvar_int_dbBptNotMonotonic;
epicsShareExtern int *pvar_int_dbTemplateMaxVars;
epicsShareExtern int *pvar_int_save_restoreDebug;
epicsShareExtern int *pvar_int_save_restoreNumSeqFiles;
epicsShareExtern int *pvar_int_save_restoreSeqPeriodInSeconds;
epicsShareExtern int *pvar_int_save_restoreIncompleteSetsOk;
epicsShareExtern int *pvar_int_save_restoreDatedBackupFiles;
epicsShareExtern int *pvar_int_save_restoreRemountThreshold;
epicsShareExtern int *pvar_int_configMenuDebug;
static struct iocshVarDef vardefs[] = {
	{"asCaDebug", iocshArgInt, (void * const)pvar_int_asCaDebug},
	{"dbRecordsOnceOnly", iocshArgInt, (void * const)pvar_int_dbRecordsOnceOnly},
	{"dbBptNotMonotonic", iocshArgInt, (void * const)pvar_int_dbBptNotMonotonic},
	{"dbTemplateMaxVars", iocshArgInt, (void * const)pvar_int_dbTemplateMaxVars},
	{"save_restoreDebug", iocshArgInt, (void * const)pvar_int_save_restoreDebug},
	{"save_restoreNumSeqFiles", iocshArgInt, (void * const)pvar_int_save_restoreNumSeqFiles},
	{"save_restoreSeqPeriodInSeconds", iocshArgInt, (void * const)pvar_int_save_restoreSeqPeriodInSeconds},
	{"save_restoreIncompleteSetsOk", iocshArgInt, (void * const)pvar_int_save_restoreIncompleteSetsOk},
	{"save_restoreDatedBackupFiles", iocshArgInt, (void * const)pvar_int_save_restoreDatedBackupFiles},
	{"save_restoreRemountThreshold", iocshArgInt, (void * const)pvar_int_save_restoreRemountThreshold},
	{"configMenuDebug", iocshArgInt, (void * const)pvar_int_configMenuDebug},
	{NULL, iocshArgInt, NULL}
};

int CentralNode_registerRecordDeviceDriver(DBBASE *pbase)
{
    const char *bldTop = "../../..";
    const char *envTop = getenv("TOP");

    if (envTop && strcmp(envTop, bldTop)) {
        printf("Warning: IOC is booting with TOP = \"%s\"\n"
               "          but was built with TOP = \"%s\"\n",
               envTop, bldTop);
    }

    if (!pbase) {
        printf("pdbbase is NULL; you must load a DBD file first.\n");
        return -1;
    }

    registerRecordTypes(pbase, 32, recordTypeNames, rtl);
    registerDevices(pbase, 53, deviceSupportNames, devsl);
    registerDrivers(pbase, 2, driverSupportNames, drvsl);
    (*pvar_func_initCentralNodeRegistrar)();
    (*pvar_func_asSub)();
    (*pvar_func_save_restoreRegister)();
    (*pvar_func_dbrestoreRegister)();
    (*pvar_func_asInitHooksRegister)();
    (*pvar_func_configMenuRegistrar)();
    (*pvar_func_caPutLogRegister)();
    (*pvar_func_cexpRegistrar)();
    (*pvar_func_iocshSystemCommand)();
    (*pvar_func_drvEvrRegister)();
    (*pvar_func_drvMrfErRegister)();
    (*pvar_func_drvMrfRegister)();
    (*pvar_func_evrEEPROMFixupRegister)();
    (*pvar_func_register_func_rebootProc)();
    (*pvar_func_register_func_scanMonInit)();
    (*pvar_func_register_func_scanMon)();
    (*pvar_func_register_func_evrTimeProcInit)();
    (*pvar_func_register_func_evrTimeProc)();
    (*pvar_func_register_func_evrTimeDiag)();
    (*pvar_func_register_func_evrTimeRate)();
    (*pvar_func_register_func_evrPatternProcInit)();
    (*pvar_func_register_func_evrPatternProc)();
    (*pvar_func_register_func_evrPatternState)();
    (*pvar_func_register_func_evrPatternSim)();
    (*pvar_func_register_func_evrPatternSimTest)();
    (*pvar_func_register_func_lsubTrigSelInit)();
    (*pvar_func_register_func_lsubTrigSel)();
    (*pvar_func_register_func_lsubEvSelInit)();
    (*pvar_func_register_func_lsubEvSel)();
    (*pvar_func_register_func_aSubEvOffsetInit)();
    (*pvar_func_register_func_aSubEvOffset)();
    (*pvar_func_register_func_aSubEvDescInit)();
    (*pvar_func_register_func_aSubEvDesc)();
    (*pvar_func_register_func_evrModifier5)();
    (*pvar_func_register_func_evrModifier5Bits)();
    iocshRegisterVariable(vardefs);
    return 0;
}

/* registerRecordDeviceDriver */
static const iocshArg registerRecordDeviceDriverArg0 =
                                            {"pdbbase",iocshArgPdbbase};
static const iocshArg *registerRecordDeviceDriverArgs[1] =
                                            {&registerRecordDeviceDriverArg0};
static const iocshFuncDef registerRecordDeviceDriverFuncDef =
                {"CentralNode_registerRecordDeviceDriver",1,registerRecordDeviceDriverArgs};
static void registerRecordDeviceDriverCallFunc(const iocshArgBuf *)
{
    CentralNode_registerRecordDeviceDriver(*iocshPpdbbase);
}

} // extern "C"
/*
 * Register commands on application startup
 */
static int Registration() {
    iocshRegisterCommon();
    iocshRegister(&registerRecordDeviceDriverFuncDef,
        registerRecordDeviceDriverCallFunc);
    return 0;
}

static int done = Registration();
