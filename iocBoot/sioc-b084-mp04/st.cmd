#!../../bin/linuxRT-x86_64/CentralNode

# ====================================================================
#
# SIOC:B084:MP04 Specific Start Up
#
# ====================================================================

< envPaths

# ====================================================================
# Setup environment variables specific to this IOC
# ====================================================================
# Keep the PV name we have in PROD
epicsEnvSet("LOCATION_INDEX","04")
epicsEnvSet("IOC_PV",   "SIOC:SYS0:MP${LOCATION_INDEX}")
epicsEnvSet("LOCATION", "B084")
epicsEnvSet("FPGA_IP",  "10.0.0.103")
epicsEnvSet("CN_INDEX", "2")
epicsEnvSet("CARD_INDEX","1")

# ====================================================================
# Load the common Central Node startup for DEV
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_dev.cmd

