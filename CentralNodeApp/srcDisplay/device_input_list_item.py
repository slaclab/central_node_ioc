from os import path
from pydm import Display
from bypass_panel import BypassPanel

class DeviceInputListItem(Display):
  _device_name = "DEVICE"
  _parent = None

  def __init__(self, device_name=None, device="DEVICE", crate="X", card="X", channel="X", parent=None):
    super(DeviceInputListItem, self).__init__(parent=parent)
    _parent = parent
    if device_name:
      _device_name = device_name
      self.ui.currentStateLabel.channel = "ca://{devname}_MPSC".format(devname=device_name)
      self.ui.latchedStateLabel.channel = "ca://{devname}_MPS".format(devname=device_name)
      self.ui.unlatchButton.channel = "ca://{devname}_UNLH".format(devname=device_name)
      self.ui.bypassValueLabel.channel = "ca://{devname}_BYPV".format(devname=device_name)
      self.ui.bypassStatusLabel.channel = "ca://{devname}_BYPS".format(devname=device_name)
      self.ui.deviceNameLabel.setText(device)
      self.ui.crateLabel.setText(str(crate))
      self.ui.cardLabel.setText(str(card))
      self.ui.channelLabel.setText(str(channel))
      self.ui.bypassButton.clicked.connect(self.handleBypassButton)

  def handleBypassButton(self):
    bypassPanel = BypassPanel(self._device_name, parent=self)
    bypassPanel.exec_()
    print "hi mom"

  def ui_filename(self):
    return 'device-input-list-item.ui'
    
  def ui_filepath(self):
    return path.join(path.dirname(path.realpath(__file__)), self.ui_filename())

intelclass = DeviceInputListItem
