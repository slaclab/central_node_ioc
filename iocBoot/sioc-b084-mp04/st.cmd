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
epicsEnvSet("IOC_PV",   "SIOC:B084:MP04")
epicsEnvSet("LOCATION", "B084")
epicsEnvSet("FPGA_IP",  "10.0.0.103")

# ====================================================================
# Load the common Central Node startup
# ====================================================================
 < ${TOP}/iocBoot/common/central_node.cmd

