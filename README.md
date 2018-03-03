Central Node IOC
----------------

This is the MPS Central Node IOC (sioc-sys2-mp01), which runs on cpu-li00-mp01.

Configuration
-------------

After startup the IOC does not automatically loads the MPS configuration, that must
be done by setting the CONFIG_LOAD PV with the database file name (in YAML format):

```
   caput SIOC:SYS2:MP01:CONFIG_LOAD mps_config-2018-02-09-a.yaml
```

The MPS configuration is kept in the $PHYSICS_TOP/mps_configuration area. In
addition to the YAML configuration file it contains auto-generated EPICS
database files (under the central_node_db subdirectory). Those databases
are loaded by the st.cmd script at startup time.

Please check the mps_configuration/README.md file for more information.
