# ====================================================================
#
# Generic MPS Central Node Start Up for All Facilities
#
# ====================================================================

# ====================================================================
# Setup some additional common environment variables
# ====================================================================
# Environment variables for IOC Admin
epicsEnvSet("ENGINEER","Jeremy Mock")

# MPS Database location
# Note: PHYSICS_TOP is defined in the facility specific script.
epicsEnvSet("MPS_ENV_DATABASE_VERSION", "${MPS_VERSION}")
epicsEnvSet("MPS_ENV_CONFIG_PATH", "${PHYSICS_TOP}/mps_configuration/${MPS_ENV_DATABASE_VERSION}")

# MPS history server configurations
# Note: MPS_ENV_HISTORY_HOST is defined in the facility specific script.
epicsEnvSet("MPS_ENV_HISTORY_PORT", "3356")
epicsEnvSet("MPS_ENV_UPDATE_TIMEOUT", "3499")

# Point 'YAML_PATH' to the yaml_fixes directory
epicsEnvSet("YAML_PATH", "${TOP}/firmware/yaml_fixes")

# Location to download the YAML file from the FPGA
epicsEnvSet("YAML_DIR","${IOC_DATA}/${IOC}/yaml")

# Yaml File (Note: The CN IOC needs this variable to be called "MPS_ENV_FW_CONFIG")
epicsEnvSet("MPS_ENV_FW_CONFIG", "${YAML_DIR}/000TopLevel.yaml")

# Defaults Yaml file
#epicsEnvSet("MPS_ENV_FW_DEFAULTS", "${YAML_DIR}/config/defaults.yaml")
epicsEnvSet("MPS_ENV_FW_DEFAULTS", "${YAML_PATH}/config/defaults.yaml")

# Use Automatic generation of records from the YAML definition
# 0 = No, 1 = Yes (using maps), 2 = Yes (using hash)
epicsEnvSet("AUTO_GEN", 0 )

# CPSW Ports
epicsEnvSet("CPSW_PORT", "CentralNodeCPSW")
epicsEnvSet("TPRTRIGGER_PORT","TPRTRIGGER_PORT")
epicsEnvSet("TPRPATTERN_PORT","TPRPATTERN_PORT")
epicsEnvSet("CROSSBARCTRL_PORT","CROSSBARCTRL_PORT")

# Dictionary file for manual (empty string if none)
epicsEnvSet("DICT_FILE", "firmware/CentralNodeFirmware.dict")

# ====================================================================
# Start from TOP
# ====================================================================
cd ${TOP}

# ====================================================================
# DBD Loading
# ====================================================================
## Register all support components
dbLoadDatabase("dbd/CentralNode.dbd")
CentralNode_registerRecordDeviceDriver(pdbbase)

# ====================================================================
# Driver setup and initialization
# ====================================================================

## Configure the Yaml Downloader Driver
# DownloadYamlFile(
#    IP Address,        # Target FPGA IP Address
#    Destination Path,  # The destination folder where the YAML file will be written
DownloadYamlFile("${FPGA_IP}", "${YAML_DIR}")

## Configure the Yaml Loader Driver
# cpswLoadYamlFile(
#    Yaml Doc,                  # Path to the YAML hierarchy description file
#    Root Device,               # Root Device Name (optional; default = 'root')
#    YAML Path,                 #directory where YAML includes can be found (optional)
#    IP Address,                # OPTIONAL: Target FPGA IP Address. If not given it is taken from the YAML file
cpswLoadYamlFile("${MPS_ENV_FW_CONFIG}", "NetIODev", "", "${FPGA_IP}")

# Load efautl configuration
cpswLoadConfigFile("${MPS_ENV_FW_DEFAULTS}", "mmio")

## Configure the Cental Node engine
# configureCentralNode(
#    Port Name,                 # Name given to this port driver
configureCentralNode("CENTRAL_NODE")

## Configure asyn port driver
# YCPSWASYNConfig(
#    Port Name,                 # Name given to this port driver
#    Root Path                  # OPTIONAL: Root path to start the generation. If empty, the Yaml root will be used
#    Record name Prefix,        # Record name prefix
#    DB Autogeneration mode,    # Set autogeneration of records. 0: disabled, 1: Enable usig maps, 2: Enabled using hash names.
#    Load dictionary,           # Dictionary file path with registers to load. An empty string will disable this function
YCPSWASYNConfig("${CPSW_PORT}", "", "", "${AUTO_GEN}", "${DICT_FILE}")

## Configure the tprTrigger driver
# tprTriggerAsynDriverConfigure(
#    Port name,                 # The name given to this port driver
#    Root path)                 # Root path to the AmcCarrierCore register area
tprTriggerAsynDriverConfigure("${TPRTRIGGER_PORT}", "mmio/AmcCarrierCore")

## Configure the tprPattern driver
# tprPatternAsynDriverConfigure(
#    Port name,                 # The name given to this port driver
#    Root path,                 # Root path to the AmcCarrierCore register area
#    Stream name)               # Name of the timing stream
#tprPatternAsynDriverConfigure("${TPRPATTERN_PORT}", "mmio/AmcCarrierCore", "tstream")

## Configure the crossbarControl driver
crossbarControlAsynDriverConfigure("${CROSSBARCTRL_PORT}", "mmio/AmcCarrierCore/AxiSy56040")

# ====================================================================
# Load application specific configurations
# ====================================================================

# ====================================================================
# Asyn masks
# ====================================================================


# ====================================================================
# Load record databases
# ====================================================================
# Load iocAdmin databases to support IOC Health and monitoring
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_PV}")
dbLoadRecords("db/iocAdminScanMon.db","IOC=${IOC_PV}")

# The following database is a result of a python parser
# which looks at RELEASE_SITE and RELEASE to discover
# versions of software your IOC is referencing
# The python parser is part of iocAdmin
dbLoadRecords("db/iocRelease.db","IOC=${IOC}")
dbLoadRecords("db/CentralNode.db","IOC=${IOC_PV},ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("db/Carrier.db","P=${IOC_PV}, PORT=${CPSW_PORT},ASG1=${ASG1},ASG2=${ASG2}")

# crossbarControl
dbLoadRecords("db/crossbarCtrl.db", "DEV=${IOC_PV},PORT=${CROSSBARCTRL_PORT}")

# tprTrigger database
dbLoadRecords("db/tprTrig.db", "PORT=${TPRTRIGGER_PORT},LOCA=${LOCATION},IOC_UNIT=MP${LOCATION_INDEX},INST=${CARD_INDEX}")

# tprPattern database
#dbLoadRecords("db/tprPattern.db", "PORT=${TPRPATTERN_PORT},LOCA=${LOCATION},IOC_UNIT=MP${LOCATION_INDEX},INST=${CARD_INDEX}")

# Save/load configuration database
dbLoadRecords("db/saveLoadConfig.db", "P=${IOC_PV}, PORT=${CPSW_PORT}")

# Central Node database, from the MPS configuration database
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/device_inputs.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/analog_devices.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/destinations.db","BASE=${IOC_PV},ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/apps.db","BASE=${IOC_PV},ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/conditions.db","BASE=${IOC_PV},ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_sc_diag0.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_sc_bsyd.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_sc_hxr.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_sc_sxr.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_sc_lesa.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/faults_laser.db","ASG1=${ASG1},ASG2=${ASG2}")
dbLoadRecords("${MPS_ENV_CONFIG_PATH}/central_node_db/cn${CN_INDEX}/fault_bypass.db","ASG1=${ASG1},ASG2=${ASG2}")

# ====================================================================
# Setup autosave/restore
# ====================================================================
set_requestfile_path("${IOC_DATA}/${IOC}/autosave-req")
set_savefile_path("${IOC_DATA}/${IOC}/autosave")
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")

# ====================================================================
# Channel Access security
# ====================================================================
# This is required if you use caPutLog.
# Set access security filea
# Load common LCLS Access Configuration File
#< ${ACF_INIT}

# Load local CAS file until after 100 MeV program
asSetFilename("${TOP}/iocBoot/common/cn_cas.acf")

# ====================================================================
# IOC Init
# ====================================================================
iocInit()

# ====================================================================
# caPutLogging
# ====================================================================
# Turn on caPutLogging:
# Log values only on change to the iocLogServer:
caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
caPutLogShow(2)

# ====================================================================
# Autosave tasks
# ====================================================================
# Wait before building autosave files
epicsThreadSleep(1)

# Generate the autosave PV list
# Note we need change directory to
# where we are saving the restore
# request file.
cd("${IOC_DATA}/${IOC}/autosave-req")
makeAutosaveFiles()
cd("${TOP}")

# Start the save_restore task
# save changes on change, but no faster
# than every 5 seconds.
# Note: the last arg cannot be set to 0
create_monitor_set("info_positions.req", 5 )
create_monitor_set("info_settings.req" , 5 )

#////////////////////////////////////////#
#Run script to generate archiver files   #
#////////////////////////////////////////#
cd(${TOP}/iocBoot/common/)
system("./makeArchive.sh ${IOC} &")
cd(${TOP})

