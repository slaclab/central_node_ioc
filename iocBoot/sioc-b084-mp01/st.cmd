#!../../bin/linuxRT-x86_64/CentralNode

# ====================================================================
#
# SIOC:B084:MP01 Specific Start Up
#
# ====================================================================

< envPaths

# ====================================================================
# Setup environment variables specific to this IOC
# ====================================================================
# Keep the PV name we have in PROD
epicsEnvSet("IOC_PV",   "SIOC:SYS0:MP01")
epicsEnvSet("LOCATION", "B084")
epicsEnvSet("FPGA_IP",  "10.0.0.102")

# ====================================================================
# Load the common Central Node startup for DEV
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_dev.cmd

