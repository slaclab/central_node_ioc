#!../../bin/linuxRT-x86_64/CentralNode

# ====================================================================
#
# SIOC:SYS0:MP03 Specific Start Up
#
# ====================================================================

< envPaths

# ====================================================================
# Setup environment variables specific to this IOC
# ====================================================================
epicsEnvSet("LOCATION_INDEX","03")
epicsEnvSet("IOC_PV",   "SIOC:SYS0:MP${LOCATION_INDEX}")
epicsEnvSet("LOCATION", "SYS0")
epicsEnvSet("FPGA_IP",  "10.0.1.102")
epicsEnvSet("CN_INDEX", "3")
epicsEnvSet("CARD_INDEX","1")
epicsEnvSet("MPS_VERSION","injector")

# Set ASG groups
epicsEnvSet("ASG1", "MCC")
epicsEnvSet("ASG2", "EOICDESK")

# ====================================================================
# Load the common Central Node startup for PROD
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_prod.cmd

dbpf ${IOC_PV}:ENABLE 0
dbpf ${IOC_PV}:EVALUATION_ENABLE 0
dbpf ${IOC_PV}:SW_ENABLE 0
dbpf ${IOC_PV}:TIMING_CHECK_ENABLE 0
dbpf ${IOC_PV}:TIMEOUT_ENABLE 0
dbpf ${IOC_PV}:SC_BSYD_MAX_PERMIT 1
