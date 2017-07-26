from os import path
from pydm import Display

class MitigationListItem(Display):
  def __init__(self, mitigation_label="MIT", mitigation_pv="DEVICE", parent=None):
    super(MitigationListItem, self).__init__(parent=parent)
    if mitigation_pv:
      self.ui.mitigationDeviceLabel.setText(mitigation_label)
      self.ui.mitigationDeviceClassLabel.channel = "ca://{devname}".format(devname=mitigation_pv)
      self.ui.mitigationDeviceDescriptionLabel.channel = "ca://{devname}.DESC".format(devname=mitigation_pv)

  def ui_filename(self):
    return 'mitigation-list-item.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = MitigationListItem
