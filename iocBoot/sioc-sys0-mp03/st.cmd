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
# Keep the PV name we have in PROD
epicsEnvSet("IOC_PV",   "SIOC:SYS0:MP03")
epicsEnvSet("LOCATION", "GUNB")
epicsEnvSet("FPGA_IP",  "10.0.1.102")

# ====================================================================
# Load the common Central Node startup for PROD
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_prod.cmd
