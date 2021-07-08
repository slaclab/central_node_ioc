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
epicsEnvSet("LOCATION_INDEX","02")
epicsEnvSet("IOC_PV",   "SIOC:SYS0:MP${LOCATION_INDEX}")
epicsEnvSet("LOCATION", "SYS0")
epicsEnvSet("FPGA_IP",  "10.0.1.103")
epicsEnvSet("CN_INDEX", "2")
epicsEnvSet("CARD_INDEX","1")

# ====================================================================
# Load the common Central Node startup for PROD
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_prod.cmd
