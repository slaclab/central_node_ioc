from os import path
from pydm import Display

class MagnetListItem(Display):
  def __init__(self, device_name=None, parent=None):
    super(MagnetListItem, self).__init__(parent=parent)
    if device_name:
      self.ui.abortButton.channel = "ca://{devname}:ABORT".format(devname=device_name)
      self.ui.actLabel.channel = "ca://{devname}:BACT".format(devname=device_name)
      self.ui.batchCheckbox.channel = "ca://{devname}:SELECT".format(devname=device_name)
      self.ui.bCtrlSlider.channel = "ca://{devname}:BCTRL".format(devname=device_name)
      self.ui.conLabel.channel = "ca://{devname}:BCON".format(devname=device_name)
      self.ui.controlComboBox.channel = "ca://{devname}:CTRL".format(devname=device_name)
      self.ui.desLineEdit.channel = "ca://{devname}:BDES".format(devname=device_name)
      unit = device_name.split(":")[-1]
      self.ui.detailButton.setText("{}...".format(unit))
      self.ui.elementLabel.channel = "ca://{devname}:MADNAME".format(devname=device_name)
      self.ui.perturbButton.channel = "ca://{devname}:FUNC".format(devname=device_name)
      self.ui.trimButton.channel = "ca://{devname}:FUNC".format(devname=device_name)
      self.ui.statusLabel.channel = "ca://{devname}:STATMSG".format(devname=device_name)
  
  def ui_filename(self):
    return 'magnet-list-item.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = MagnetListItem