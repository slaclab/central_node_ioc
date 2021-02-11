#==============================================================
#
#  Abs:  Start the autosave task (for a soft IOC)
#
#  Name: start_restore_soft.cmd
#
#  Side: load this script after iocInit
#
#       "In the lore that has built up around autosave, 
#        PV's that should be restored only before record
#        initialization have been termed "positions".
#        All other PV's have been termed "settings".
#
#        Thus, you might run across a file called
#        'auto_positions.req', and now you'll know that 
#        the parameters in this file are intended to be 
#        restored only during autosave's pass-0."
#           - from autosave documentation
#
#  Facility:  LCLS LCAMAC Controls
#
#  Auth: 27-Jan-2011, Kristi Luchini  (LUCHINI)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        dd-mmm-yyyy, First Lastname  (USERNAME):
#          comment
#
#==============================================================
#
# Wait before building autosave files
epicsThreadSleep(1)

# Generate the autosave PV list
# Note we need change directory to 
# where we are saving the restore 
# request file.
cd("${IOC_DATA}/${IOC}/autosave-req")
pwd()
makeAutosaveFiles()


# Start the save_restore task 
# save changes on change, but no faster
# than every 5 seconds.
# Note: the last arg cannot be set to 0
create_monitor_set("info_positions.req", 5 )
create_monitor_set("info_settings.req" , 5 )

# change directory to TOP of application
cd("${TOP}")
pwd()

# End of script
