Central Node IOC
----------------

This is the MPS Central Node IOC (sioc-sys2-mp01), which runs on cpu-li00-mp01.

Configuration
-------------

After startup the IOC does not automatically loads the MPS configuration, that must
be done by setting the CONFIG_LOAD PV with the database file name:

'''
   caput SIOC:SYS2:MP01:CONFIG_LOAD mps_config-2018-02-09-a.yaml
'''