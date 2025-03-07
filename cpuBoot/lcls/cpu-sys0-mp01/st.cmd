#==============================================================
#
#  Abs:  Startup Script for cpu-sys0-mp01
#
#  Name: st.cmd
#
#  Desc: The processor is an Advantech 1U server,
#        SKY-8100 industrial pc/server.
#
#  Facility:  LCLS Shared Platform
#
#  Auth: dd-mmm-yyyy, Author's Name   (USERNAME)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        13-Jun-2020, K. Luchini      (LUCHINI):
#         add fcom setup for fast feedback
#        11-Jun-2020, K. Luchini      (LUCHINI):
#         extracted from $EPICS_IOCS/cpu-bsyh-sp01/kernel-modules.cmd
#         and renamed to st.cmd
#
#==============================================================

# =========================================
# Turn on local IPMI Client support
# =========================================
echo "Load IPMI..."
modprobe ipmi_devintf
ipmitool lan print

# =========================================
# Network configuration 
# =========================================
# Enable Ethernet SFP+ Top For Advantech 1U
ifconfig eth5 10.0.1.1 netmask 255.255.255.0 up

# ==================================================================
# escalating RT priority for network kernel threads (SCHED_FIFO, 84)
# =================================================================
for PID in  $(ps -elF | grep eth5 | grep -v grep | awk '{print $4}'); do chrt -f -p 84 $PID ; done

# End of script'


