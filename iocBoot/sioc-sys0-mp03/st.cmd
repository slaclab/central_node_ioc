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
epicsEnvSet("CN_INDEX", "3")
epicsEnvSet("LOCATION", "SYS0")
epicsEnvSet("MPS_VERSION","current")
epicsEnvSet("SLOT","2")

# ====================================================================
# Load the common Central Node startup for PROD
# ====================================================================
 < ${TOP}/iocBoot/common/central_node_prod.cmd

dbpf ${IOC_PV}:ENABLE 0
dbpf ${IOC_PV}:EVALUATION_ENABLE 0
dbpf ${IOC_PV}:SW_ENABLE 0
dbpf ${IOC_PV}:TIMING_CHECK_ENABLE 0
dbpf ${IOC_PV}:TIMEOUT_ENABLE 0
