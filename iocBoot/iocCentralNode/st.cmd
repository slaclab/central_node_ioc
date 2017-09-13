#!../../bin/rhel6-x86_64/CentralNode

## You may have to change CentralNode to something else
## everywhere it appears in this file

< envPaths

epicsEnvSet("IOC","sioc-sys2-mp01")
epicsEnvSet("IOC_PV","SIOC:SYS2:MP01")

cd ${TOP}

## Register all support components
dbLoadDatabase("dbd/CentralNode.dbd")
CentralNode_registerRecordDeviceDriver(pdbbase)

# ====================================================================
# Setup some additional environment variables
# ====================================================================
# Setup environment variables
epicsEnvSet("ENGINEER","Curly Johnson")
epicsEnvSet("LOCATION","Twilight Zone")

# END: Additional environment variables
# ====================================================================
#epicsEnvSet("MPS_ENV_CONFIG_PATH", "/afs/slac/g/lcls/epics/iocTop/users/lpiccoli/central_node_engine/src/test/yaml/")
epicsEnvSet("MPS_ENV_CONFIG_PATH", "/data/cpu-b084-hp03/lpiccoli/mps/config")
epicsEnvSet("MPS_ENV_FW_CONFIG", "/data/cpu-b084-hp03/lpiccoli/mps/CentralNodeYAML/000TopLevel.yaml")

configureCentralNode("CENTRAL_NODE")

########################################################################
# BEGIN: Load the record databases
#######################################################################
# =====================================================================
# Load iocAdmin databases to support IOC Health and monitoring
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC}")
dbLoadRecords("db/iocAdminScanMon.db","IOC=${IOC}")

# The following database is a result of a python parser
# which looks at RELEASE_SITE and RELEASE to discover
# versions of software your IOC is referencing
# The python parser is part of iocAdmin
dbLoadRecords("db/iocRelease.db","IOC=${IOC}")
dbLoadRecords("db/CentralNode.db","IOC=${IOC_PV}")
dbLoadRecords("db/device_inputs.db","BASE=MPS:DIGITAL")
dbLoadRecords("db/analog_devices.db","BASE=MPS:ANALOG")
dbLoadRecords("db/mitigation.db","BASE=${IOC_PV}")
dbLoadRecords("db/faults.db","BASE=MPS:FAULT")

iocInit()

# =====================================================
# Turn on caPutLogging:
# Log values only on change to the iocLogServer:
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
caPutLogShow(2)
# =====================================================
