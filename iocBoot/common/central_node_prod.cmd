# ====================================================================
#
# Generic MPS Central Node Start Up for PROD
#
# ====================================================================

# ====================================================================
# Setup environment variables specific to the PROD environment
# ====================================================================
# Set ASG groups
epicsEnvSet("ASG1", "MCC")
epicsEnvSet("ASG2", "EOICDESK")

# MPS Database location
epicsEnvSet("PHYSICS_TOP", "/usr/local/lcls/physics")

# MPS history server configurations
epicsEnvSet("MPS_ENV_HISTORY_HOST", "lcls-daemon2")

# ====================================================================
# Load the common Central Node startup
# ====================================================================
 < ${TOP}/iocBoot/common/central_node.cmd

