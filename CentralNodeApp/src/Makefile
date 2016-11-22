TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# ================================================
# Build the IOC application
# ================================================
# This will attempt to build an IOC application 
# for every target including the host computer.
PROD_IOC = CentralNode

# Use the following for RTEMS Only IOCs
#PROD_IOC_RTEMS = CentralNode

# CentralNode.dbd will be created and installed
DBD += CentralNode.dbd

# CentralNode.dbd will be made up from these files:
CentralNode_DBD += Init.dbd
CentralNode_DBD += base.dbd
CentralNode_DBD += devIocStats.dbd
CentralNode_DBD += autosaveSupport.dbd
CentralNode_DBD += caPutLog.dbd
CentralNode_DBD += asyn.dbd
CentralNode_DBD += $(CEXP_DBDS)

# ================================================================================
# The system command will be mainly used by linux hosted IOCs
# Very useful for changing kernel thread priorities for kernel drivers
# ================================================================================
CentralNode_DBD += system.dbd

# If you don't have an EVR comment the following two lines:
#CentralNode_DBD += evrSupport.dbd
#CentralNode_DBD += devMrfEr.dbd

# ======================================================================
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
CentralNode_SRCS += CentralNode_registerRecordDeviceDriver.cpp
CentralNode_SRCS += CentralNodeDriver.cc Init.cc

CentralNode_SRCS_DEFAULT += CentralNodeMain.cpp

CentralNode_SRCS_vxWorks += -nil-

# ========================================================================
# ========================================================================
# Add locally compiled object code
# For example, subroutines
# ========================================================================
# CentralNode_SRCS += file1.c
# CentralNode_SRCS += file2.c

# =====================================================
# Link in the code from other libraries
# =====================================================
CentralNode_LIBS += asyn

# Needed to link in iocAdmin support
CentralNode_LIBS += devIocStats

# Needed to link in autosave support
CentralNode_LIBS += autosave

# Needed to link caPutLog support
CentralNode_LIBS += caPutLog

# Needed to link cexpsh() support
CentralNode_LIBS += $(CEXP_LIBS)

CentralNode_LIBS += $(EPICS_BASE_IOC_LIBS)

#======================================================

USR_CPPFLAGS += -DLOG_ENABLED
USR_CPPFLAGS += $(EASYLOGGINGPP_INCLUDE)
USR_CPPFLAGS += $(YAML_CPP_INCLUDE)

# =========================================================================
#  Link in external package libraries (-lcentral_node_engine)
# =========================================================================
YAML_CPP_DIR=$(YAML_CPP_LIB)
CENTRAL_NODE_ENGINE_DIR=$(CENTRAL_NODE_ENGINE_LIB)
USR_CPPFLAGS += $(CENTRAL_NODE_ENGINE_INCLUDE)
#CentralNode_LIBS += central_node_engine
USR_LDFLAGS += -L$(CENTRAL_NODE_ENGINE_LIB)
USR_LDFLAGS += -L$(YAML_CPP_LIB)
CentralNode_SYS_LIBS += central_node_engine
CentralNode_SYS_LIBS += yaml-cpp

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

