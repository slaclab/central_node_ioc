from os import path
from pydm import Display

class FaultListItem(Display):
  def __init__(self, fault_description="Fault Description...", fault_pv="DEVICE", parent=None):
    super(FaultListItem, self).__init__(parent=parent)
    if fault_pv:
      self.ui.faultLabel.setText(fault_description)
      self.ui.faultStateLabel.channel = "ca://{devname}".format(devname=fault_pv)
      self.ui.faultIgnoredStateLabel.channel = "ca://{devname}_IGN".format(devname=fault_pv)
      self.ui.faultLatchedLabel.channel = "ca://{devname}_MPS".format(devname=fault_pv)
      self.ui.unlatchButton.channel = "ca://{devname}_UNLH".format(devname=fault_pv)

  def ui_filename(self):
    return 'fault-list-item.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = FaultListItem
