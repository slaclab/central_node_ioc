# ====================================================================
#
# Generic MPS Central Node Start Up for DEV
#
# ====================================================================

# ====================================================================
# Setup environment variables specific to the DEV environment
# ====================================================================
# MPS Database location
epicsEnvSet("PHYSICS_TOP", "/afs/slac/g/lcls/physics")

# MPS history server configurations
epicsEnvSet("MPS_ENV_HISTORY_HOST", "lcls-dev3")

# ====================================================================
# Load the common Central Node startup
# ====================================================================
 < ${TOP}/iocBoot/common/central_node.cmd

