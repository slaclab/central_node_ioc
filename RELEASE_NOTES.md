# RELEASE_NOTES

Release notes for the LCLS-II MPS central node IOC.

## Releases:
* __central_node_ioc-R4-7-1__:
  * Add support for cpuBoot from this app

* __central_node_ioc-R4-7-0__:
  * build against central_node_engine-R4-7-0
  * Migrate from rhel7 to rhel9 as host architecture:
    * Upgrade module versions
    * Add support for tirpc when building rhel9 targets 

* __central_node_ioc-R4-6-0__:
  * build against central_node_engine-R4-6-0

* __central_node_ioc-R4-5-4__:
  * Fix scal val type of beam class definitions - they need to be scalVal not scalVal_RO

* __central_node_ioc-R4-5-3__:
  * Move Beam Class PVs to be readback only instead of read/write to 
    address problem where some of them read 0

* __central_node_ioc-R4-5-2__:
  * Load db files for Seq status and autosave status

* __central_node_ioc-R4-5-1__:
  * Build against central_node_engine-R4-5-1:
    * Fix bug with ignore condition for devices when app is disabled

* __central_node_ioc-R4-5-0__:
  * Build against central_node_engine-R4-5-0

* __central_node_ioc-R4-4-0__:
  * Build against central_node_engine-R4-4-0
  * Add flag and PVs for active / not active logic (does nor drive an ignore condition)
    * Useful for GUI only
  * Add PVs for management of mode switching

* __central_node_ioc-R4-3-0__:
  * Build against central_node_engine-R4-3-0
  * Remove DB loads from st.cmd for database files that are no longer generated

* __central_node_ioc-R4-2-1__:
  * Reactivate CAS for MAX_PERMIT

* __central_node_ioc-R4-2-0__:
  * Build against central_node_engine-R4-2-0
    * Add status for device / fault / app card being disconnected
    * Add status for device / fault / app card being desired offline (mode switch)
  * Add PV _FLT_TEST (temporary name) to publish worst case fault state db id of fault - for gui
  * Change MAX_PERMIT PV to be an mbbi that allows eoicdesk to specify maximum operating beam class
  * Upgrade FW to 4.8.6 (no changes to CN, but for consistency)

* __central_node_ioc-R4-1-1__:
  * Build against central_node_engine-R4-1-0
    * Remove LASER from MAX permit constraint.

* __central_node_ioc-R4-1-0__:
  * Build against central_node_enging-R4-1-0 to apply maximum BC to all destinations instead of just SC_BSYD
  * add ASG to cn1 and cn2
  * Point all prod IOCs to "current" release

* __central_node_ioc-R4-0-2__:
  * Build against central_node_enging-R4-0-1 to fix analog ignore condition FW reload

* __central_node_ioc-R4-0-1__:
  * Change :CONFIG_LOAD ASG to EOICDESK
  * Add dbpf command at end of st.cmd in sioc-sys0-mp03
    to force 120 Hz only mode on boot up

* __central_node_ioc-R4-0-0__:
  * First version for 100 MeV Program
  * Build against central_node_engine-R4-0-0
  * Change ASG for some PVs
  * Create an MPS ASG for the 100 MeV PV
  * Create the 100 MeV PV
  * Fix bug with bypass exp timer for analog faults
  * Move where the ASG is defined in startup file
  * Run sioc-sys0-mp03 with "injector" configuration

* __central_node_ioc-R3-7-0__:
  * Build against central_node_engine-R3-3-0
  * Upgrade FW to R4.8.3
    * Allows central node to connect to 2 other central nodes
    * Add some PVs to support enabling and reading back status of central node chain.
  * Add _FLT PV that is true when the fault is faulted and false when it is not

* __central_node_ioc-R3-6-0__:
  * Build against central_node_engine-R3-2-0
    * Implement 1 second gate around unlatch all command - can be pressed just once
      per second
    * Fix problem with force_bc and soft_permit talking to same asyn parameter:
      * Can now revoke permit and not overwrite it with force BC

* __central_node_ioc-R3-5-0__:
  * New FW version to fix MpsTimingCheck (R4.8.0)

* __central_node_ioc-R3-4-0__:
  * Add register for reading MSB of BeamFaultReason from MpsTimingCheck
  * New FW to fix BC transition to stop facility fault
  * Add PV to read latching TimningRx fault

* __central_node_ioc-R3-3-1__:
  * Fix bug in loading of CAS

* __central_node_ioc-R3-3-0__:
  * Add Channel Access Security for inital running
  * Add unlatch all function call to end of loadConfig()

* __central_node_ioc-R3-2-2__:
  * Add script for auto archive

* __central_node_ioc-R3-2-1__:
  * Build against central_node_engine-R3-1-1
      * Fix bug for multiple ignore conditions

* __central_node_ioc-R3-2-0__:
  * New FW version to fix problem with MpsTimingMonitor
  * Add counters to monitor MpsTimingMonitor errors

* __central_node_ioc-R3-1-0__:
  * Add additional diagnostic counter PVs
  * Add alarm limits to some scal val register inputs
  * build against central_node_engine-R3-0-0

* __central_node_ioc-R3-0-0__:
  * Build against central_node_engine-R2-6-0 which fixes a problem in history sender
  * Load additional database files related to faults to handle per destination faults from autogen
  * Add additional PVs to monitor CN status (bandwidth on PGP lanes, etc.)

* __central_node_ioc-R2-5-0__:
  * Build against central_node_engine-R2-5-0 which adds support for digital ignore conditions
  * Upgrade timing modules and timing package per timing team
  * Stop loading fault_states.db - this is no longer needed
  * Upgrade to FW version R4.4.9

* __central_node_ioc-R2-4-0__:
  * Remove fault state PVs
  * Remove unlatching of faults
  * build against central_node_engine_R2-4-0

* __central_node_ioc-R2-3-0__:
  * Add function to Central Node Driver to allow force any destination to any power class
    * No new PVs - the PVs will be generated for each destination by mps_database

* __central_node_ioc-R2-2-1__:
  * Update `central_node_engine` to version `central_node_engine-R2-2-1`.

* __central_node_ioc-R2-2-0__:
  * Update `central_node_engine` to version `central_node_engine-R2-2-0`.
  * Change unlatch all function to unlatch soft faults and FW faults.

* __central_node_ioc-R2-1-1__:
  * Update `central_node_engine` to version `central_node_engine-R2-1-1`.

* __central_node_ioc-R2-1-0__:
  * Update `central_node_engine` to version `central_node_engine-R2-1-0`.
  * Add support to read and write the app timeout enable bits.

* __central_node_ioc-R2-0-0__:
  * Update FW to version `7ef9f2d`.
  * Update the central_node_engine to version `central_node_engine-R2-0-0`.
  * Update `MpsCentralNodeConfig.yaml` to support all 1024 apps.
  * Improve evaluation processing pipeline.
  * Fixes/Improvements for the dual CN operation.
  * Cleanup `sioc-sys0-mp01` startup script.
  * Expose Pgp2bAxi counter for all 14 channels.
  * Create second DEV IOC for the dual-carrier configuration.
  * Add support for the `yamlDownloader` module, and use it to download the
    YAML files from the FPGA, instead of a local copy of the YAML files.
  * Add the `yaml_fixes` directory to hold local patched YAML files,
    and point CPSW's `YAML_DIR` env var to it.
  * Load the database from the `PHYSICS_TOP` location.
  * Fix a DESC field which was too long.
  * Update the YCPSWASYN register mapping, according to changes in the new
    YAML files.
  * Add new options to the `mps` shell command, to get information about
    the power class changes info.

* __central_node_ioc-R1-5-0__:
  * Merge changes from the EIC branch.
  * Update buildroot to version 2019.08.
  * Update CPSW to version R4.4.1. This implied update boost to version
    1.64.0 and yaml-cpp to version yaml-cpp-0.5.3_boost-1.64.0.
  * Update central_node_engine to version central_node_engine-R1-7-0.
  * Update EPICS base to version R7.0.3.1-1.0. This implied updating
    all EPICS modules to the latest version:
    - asyn too version R4.32-1.0.0.
    - seq to version R2.2.4-1.2.
    - autosave to version R5.8-2.1.0.
    - caPutLog to version R3.7-1.0.0.
    - iocAdmin to version R3.1.16-1.3.2.
    - miscUtils to version R2.2.5.
    - ycpswasyn to version R3.3.3.
    - yamlLoader to version R2.1.0.
  * Fix all compilation warnings.
  * Fix bug in YAML file, which is caught by the new version of CPSW.
  * Fix path to the DEV version of PHYSICS_TOP.
  * Rename SYS2 to SYS0.
  * Add the display directory to the gitignore file.

* __central_node_ioc-R1-4-2-eic (EIC branch)__:
  * Update MCS file with version loaded in EIC.

* __central_node_ioc-R1-4-1-eic (EIC branch)__:
  * Renamed SYS2 to SYS0

* __central_node_ioc-R1-4-0-eic (EIC branch)__:
  * Build agaist central_node_engine-R1-6-0-eic (branch)
  * New central_node_engine has bug fixes for AOM/Shutter control
  * The UNLATCHALL PV now also clears FW mitigation latched while
    enabling AOM.

* __central_node_ioc-R1-3-1-eic (EIC branch)__:
  * Addition of PV for disabling AOM $(IOC):AOM_DISABLE

* __central_node_ioc-R1-3-0-eic (EIC branch)__:
  * Build against central_node_engine-1-5-0-eic (branch)
  * Addition of $(IOC):LINAC_TM_PC containing PC sent to timing

* __central_node_ioc-R1-2-12__:
  * Build against central_node_engine-R1-4-1.

* __central_node_ioc-R1-2-11__:
  * Build against central_node_engine-R1-4-0.

* __central_node_ioc-R1-2-10__:
  * Addition of $(IOC):DISABLE_BEAM PV: this allows operators to
    disable the gun shutter.

* __central_node_ioc-R1-2-9__:
  * Build against central_node_engine-R1-3-1. MPS is not enabled
    after configuration is loaded the first time.

* __central_node_ioc-R1-2-8__:
  * Build against central_node_engine-R1-3-0
  * Addition of PV to clear beam fault violations [$(IOC):BEAM_FAULT_CLEAR]

* __central_node_ioc-R1-2-7__:
  * Addition of support for FaultState PVs
  * st.cmd now loads the file fault_states.db (for FaultState PVs)
  * Version compatible with mps_config-2018-09-10-a, earlier
    mps_config versions don't have the fault_states.db file

* __central_node_ioc-R1-2-6__:
  * Version built against central_node_engine-R1-2-4, minor fix
    to the beam charge verification table readback ('mps show fw'
    command)

* __central_node_ioc-R1-2-5__:
  * Version built against central_node_engine-R1-2-3, the only
    difference to previous version is that the beam charge table
    is now populated with values from the configuration database
    (as opposed to hardcoded test numbers).

* __central_node_ioc-R1-2-4__:
  * Add PVs to control and readback the status of the timing core.
  * Rename IOC directory to sioc-sys2-mp01.
  * Update Central Node FPGA IP address to 10.0.0.102.
  * Give executable permission to st.cmd.
  * Update PHYSICS_TOP path used in PROD.
  * Use dictionary file which is shipped with the IOC application.
  * Add PVs to save and load register configuration via CPSW YAML.
  * Rename the directory yaml to firmware.
  * Add the FW mcs file to the IOC app so it can be (re)load in PROD.
  * Add the YAML definition file corresponding to the current FW image
    to the IOC application and reference that file from st.cmd.

* __central_node_ioc-R1-2-3__:
  * Fixes to the bypass string PVs, each analog threshold
    has a separate bypass string - which is indicated by
    the INP address calculated by the scripts that export
    information from the MPS database. When number of seconds
    reaches zero the string is set back to 'Not Bypassed'

* __central_node_ioc-R1-2-2__:
  * build against central_node_engine-R1-2-2
  * Addition (again) of code to retrieve status of ignore
    condition for analog integrator (used for TORO DIFF
    for EIC)

* __central_node_ioc-R1-2-1__:
 * built against central_node_engine-R1-2-1

* __central_node_ioc-R1-2-0__:
  * built against central_node_engine-R1-2-0
  * not compatible with R1-1-x

* __central_node_ioc-R1-1-0__:
  * built against central_node_engine-R1-1-0

* __central_node_ioc-R1-0-0__:
  * first version for EIC
