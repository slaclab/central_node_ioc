from os import path
from pydm import Display

class PowerClassListItem(Display):
  def __init__(self, power_class_pv="DEVICE", parent=None):
    super(PowerClassListItem, self).__init__(parent=parent)
    if power_class_pv:
      self.ui.powerClassNumberLabel.channel = "ca://{devname}".format(devname=power_class_pv)
      self.ui.powerClassDescriptionLabel.channel = "ca://{devname}.DESC".format(devname=power_class_pv)

  def ui_filename(self):
    return 'power-class-list-item.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = PowerClassListItem
