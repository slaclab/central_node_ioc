#!iocSpecificRelease/bin/linuxRT-x86_64/CentralNode

## You may have to change CentralNode to something else
## everywhere it appears in this file

< envPaths

#epicsEnvSet("TOP", "/afs/slac/g/lcls/epics/iocTop/users/lpiccoli/central_node_ioc")
epicsEnvSet("IOC","sioc-sys2-mp01")
epicsEnvSet("IOC_PV","SIOC:SYS2:MP01")

pwd()
cd ${TOP}
pwd()

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
epicsEnvSet("MPS_ENV_DATABASE_VERSION", "current")

epicsEnvSet("PHYSICS_TOP", "/afs/slac/g/lcls/physics")
epicsEnvSet("MPS_ENV_CONFIG_VERSION", "mps_configuration-R1-0-0")
epicsEnvSet("MPS_ENV_CONFIG_PATH", "${PHYSICS_TOP}/mps_configuration/${MPS_ENV_DATABASE_VERSION}")
epicsEnvSet("MPS_ENV_FW_CONFIG", "/data/${IOC}/yaml/000TopLevel.yaml")
epicsEnvSet("MPS_ENV_HISTORY_HOST", "lcls-dev3")
epicsEnvSet("MPS_ENV_HISTORY_PORT", "3356")
epicsEnvSet("MPS_ENV_UPDATE_TIMEOUT", "3499")

# Yaml File
epicsEnvSet("YAML_FILE", "${MPS_ENV_FW_CONFIG}")

# Central Node FPGA IP address 
epicsEnvSet("FPGA_IP", "10.0.0.102")
#epicsEnvSet("FPGA_IP", "10.0.1.103")

# Use Automatic generation of records from the YAML definition
# 0 = No, 1 = Yes (using maps), 2 = Yes (using hash)
epicsEnvSet("AUTO_GEN", 0 )

# CPSW Port
epicsEnvSet("CPSW_PORT", "CentralNodeCPSW")

# Dictionary file for manual (empty string if none)
epicsEnvSet("DICT_FILE", "/data/${IOC}/CentralNodeFirmware.dict")

# ===================================================================================================================
# Driver setup and initialization for YCPSWAsyn
# ===================================================================================================================

## Configure the Yaml Loader Driver
# cpswLoadYamlFile(
#    Yaml Doc,                  # Path to the YAML hierarchy description file
#    Root Device,               # Root Device Name (optional; default = 'root')
#    YAML Path,                 #directory where YAML includes can be found (optional)
#    IP Address,                # OPTIONAL: Target FPGA IP Address. If not given it is taken from the YAML file
# ==========================================================================================================
cpswLoadYamlFile("${YAML_FILE}", "NetIODev", "", "${FPGA_IP}")

configureCentralNode("CENTRAL_NODE")

## Configure asyn port driver
# YCPSWASYNConfig(
#    Port Name,                 # the name given to this port driver
#    Root Path                  # OPTIONAL: Root path to start the generation. If empty, the Yaml root will be used
#    Record name Prefix,        # Record name prefix
#    DB Autogeneration mode,    # Set autogeneration of records. 0: disabled, 1: Enable usig maps, 2: Enabled using hash names.
#    Load dictionary,           # Dictionary file path with registers to load. An empty string will disable this function
# ==========================================================================================================
YCPSWASYNConfig("${CPSW_PORT}", "", "", "${AUTO_GEN}", "${DICT_FILE}")

########################################################################
# BEGIN: Load the record databases
#######################################################################
# =====================================================================
# Load iocAdmin databases to support IOC Health and monitoring
# =====================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_PV}")
dbLoadRecords("db/iocAdminScanMon.db","IOC=${IOC_PV}")

# The following database is a result of a python parser
# which looks at RELEASE_SITE and RELEASE to discover
# versions of software your IOC is referencing
# The python parser is part of iocAdmin
dbLoadRecords("db/iocRelease.db","IOC=${IOC}")
dbLoadRecords("db/CentralNode.db","IOC=${IOC_PV}")
dbLoadRecords("db/Carrier.db","P=${IOC_PV}, PORT=${CPSW_PORT}")

dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/device_inputs.db")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/analog_devices.db")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/destinations.db","BASE=${IOC_PV}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/faults.db")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/apps.db","BASE=${IOC_PV}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/conditions.db","BASE=${IOC_PV}")

cd iocBoot/sioc-sys2-mp01

#======================================================================
# Save/Restore 
#======================================================================
set_requestfile_path("${IOC_DATA}/${IOC}/autosave-req")
set_savefile_path("/${IOC_DATA}/${IOC}/autosave")
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")

iocInit()

# =====================================================
# Turn on caPutLogging:
# Log values only on change to the iocLogServer:
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
caPutLogShow(2)
# =====================================================

# Start autosave
< start_restore_soft.cmd
