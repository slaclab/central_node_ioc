TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

#==================================================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
#==================================================================================

# Support for CPSW
USR_INCLUDES = -I$(BOOST_INCLUDE) -I$(CPSW_FRAMEWORK_INCLUDE) -I$(YAML_INCLUDE)
#==================================================================================

#======================================================
# PATH TO "NON EPICS" EXTERNAL PACKAGES: USER LIBRARIES
#======================================================
# Support for CPSW
cpsw_DIR = $(CPSW_FRAMEWORK_LIB)
boost_system_DIR = $(BOOST_LIB)
yaml-cpp_DIR = $(YAML_LIB)

# Central Node Engine Lib
USR_INCLUDES += -I$(CENTRAL_NODE_ENGINE_INCLUDE)
central_node_engine_DIR = $(CENTRAL_NODE_ENGINE_LIB)

# ======================================================
# LINK "NON EPICS" EXTERNAL PACKAGE LIBRARIES STATICALLY
#=======================================================
USR_LIBS_Linux += central_node_engine cpsw boost_system yaml-cpp

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
CentralNode_DBD += asSupport.dbd
CentralNode_DBD += caPutLog.dbd
CentralNode_DBD += asyn.dbd
CentralNode_DBD += ycpswasyn.dbd
CentralNode_DBD += yamlLoader.dbd
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

CentralNode_LIBS += ycpswasyn
CentralNode_LIBS += yamlLoader

# Needed to link cexpsh() support
CentralNode_LIBS += $(CEXP_LIBS)

CentralNode_LIBS += $(EPICS_BASE_IOC_LIBS)

#======================================================

#USR_CPPFLAGS += -DLOG_ENABLED
#USR_CPPFLAGS += -DLOG_STDOUT
USR_CPPFLAGS += -I$(EASYLOGGINGPP_INCLUDE)

# =========================================================================
#  Link in external package libraries (-lcentral_node_engine)
# =========================================================================
YAML_DIR=$(YAML_LIB)
CPSW_DIR=$(CPSW_LIB)
CENTRAL_NODE_ENGINE_DIR=$(CENTRAL_NODE_ENGINE_LIB)
USR_LDFLAGS += -L$(CENTRAL_NODE_ENGINE_LIB)
USR_LDFLAGS += -L$(YAML_LIB)
USR_LDFLAGS += -L$(CPSW_FRAMEWORK_LIB)
USR_LDFLAGS += -L$(BOOST_LIB)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

