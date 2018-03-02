#!/bin/bash

# Define LCLS_ROOT, based on AFS for development or NFS for production
if [ -d /afs/slac/g/lcls ]; then
        export LCLS_ROOT=/afs/slac/g/lcls
else
        export LCLS_ROOT=/usr/local/lcls
fi
# Override EPICS_TOP location
export EPICS_TOP=$LCLS_ROOT/epics/R3-14-12-4_1-0

export EPICS_BASE_VER=base-R3-14-12-4_1-0
export EPICS_EXTENSIONS_VER=R3-14-12
export EPICS_MODULES_VER=

export EPICS_EXTENSIONS=$EPICS_TOP/extensions-$EPICS_EXTENSIONS_VER
export EPICS_MODULES_TOP=${EPICS_TOP}/modules
export MOD=$EPICS_MODULES_TOP
export EPICS_IOC_TOP=${EPICS_TOP}/iocTop
export PYEPICS_LIBCA=$EPICS_BASE_TOP/$EPICS_BASE_VER/lib/$EPICS_HOST_ARCH/libca.so
#source ${LCLS_ROOT}/tools/script/ENVS_dev3.bash


# Source group environment setup for EPICS
if [ -f ${LCLS_ROOT}/epics/setup/epicsReset_dev3.bash ]; then
    . ${LCLS_ROOT}/epics/setup/epicsReset_dev3.bash
fi

export EPICS_BASE_TOP=$LCLS_ROOT/epics/base
export PACKAGE_TOP=/afs/slac/g/lcls/package
export EPICS_MBA_TEMPLATE_TOP=${EPICS_MODULES_TOP}/icdTemplates/icdTemplates-R1-2-0

#Override the python path to use 2.7.9 x86-64:
export PATH=$PACKAGE_TOP/python/python2.7.9/linux-x86_64/bin/:$PATH
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PACKAGE_TOP/python/python2.7.9/linux-x86_64/lib

#Add the pydm widgets directory to to the PyQt Designer plugin path
export PYQTDESIGNERPATH=/u/ad/mgibbs/pydm:$PYQTDESIGNERPATH
export PYDM_PATH=/u/ad/mgibbs/pydm
#Add Qt 4.8.6 binaries to the path
export PATH=$PACKAGE_TOP/Qt/4.8.6/bin/:$PATH

#Add PV Gateway to the EPICS Address List, so we can get (read only) production PVs on dev
export EPICS_CA_ADDR_LIST="134.79.219.255 134.79.151.21:5068"
#Use newer gcc toolchain libs
#export LD_LIBRARY_PATH=/u/ad/mgibbs/toolchains/lib:/u/ad/mgibbs/toolchains/lib64:$LD_LIBRARY_PATH
